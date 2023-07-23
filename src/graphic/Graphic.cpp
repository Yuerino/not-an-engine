#include "graphic/Graphic.hpp"

#include <iostream>

#include "GlfwApi.hpp"
#include "graphic/CommandBuffer.hpp"
#include "util.hpp"

namespace nae {

Graphic::Graphic(Window &window)
    : window_(window),
      instance_{{}, graphic::Instance::getGlfwRequiredExtensions()},
      surface_{instance_, window_},
      physicalDevice_{instance_, surface_, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}},
      device_{physicalDevice_, surface_},
      pSwapChain_{std::make_unique<graphic::SwapChain>(
              physicalDevice_,
              surface_,
              device_,
              vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc)},
      pipeline_{device_, *pSwapChain_, util::vertShaderPath, util::fragShaderPath},
      vkCommandPool_{device_.get(),
                     vk::CommandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                               device_.getGraphicQueueFamilyIndex()}},
      vkCommandBuffers_{
              device_.get(),
              vk::CommandBufferAllocateInfo{*vkCommandPool_, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT}} {
    pSwapChain_->createFrameBuffers(device_, pipeline_.getRenderPass());

    imageAcquiredSemaphores_.reserve(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores_.reserve(MAX_FRAMES_IN_FLIGHT);
    drawFences_.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        imageAcquiredSemaphores_.emplace_back(device_.get(), vk::SemaphoreCreateInfo{});
        renderFinishedSemaphores_.emplace_back(device_.get(), vk::SemaphoreCreateInfo{});
        drawFences_.emplace_back(device_.get(), vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
    }
}

Graphic::~Graphic() {
    device_.get().waitIdle();
}

void Graphic::Update() {
    // Acquire next image
    vk::Result result;
    uint32_t imageIndex;
    std::tie(result, imageIndex) = pSwapChain_->get().acquireNextImage(std::numeric_limits<uint64_t>::max(),
                                                                       *imageAcquiredSemaphores_[currentFrame_]);
    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Command buffer begin
    vkCommandBuffers_[currentFrame_].reset(vk::CommandBufferResetFlags{});
    vkCommandBuffers_[currentFrame_].begin(vk::CommandBufferBeginInfo{});

    // Render pass begin
    vk::ClearValue clearValue{vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::RenderPassBeginInfo renderPassBeginInfo{*pipeline_.getRenderPass().get(),
                                                *pSwapChain_->getFrameBuffers()[imageIndex],
                                                vk::Rect2D{vk::Offset2D{0, 0}, pSwapChain_->getExtent()},
                                                1,
                                                &clearValue};
    vkCommandBuffers_[currentFrame_].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    // Bind dynamic view port and scissor
    vk::Viewport viewPort{0.0f,
                          0.0f,
                          static_cast<float>(pSwapChain_->getExtent().width),
                          static_cast<float>(pSwapChain_->getExtent().height),
                          0.0f,
                          1.0f};
    vk::Rect2D scissor{vk::Offset2D{0, 0}, pSwapChain_->getExtent()};
    vkCommandBuffers_[currentFrame_].setViewport(0, viewPort);
    vkCommandBuffers_[currentFrame_].setScissor(0, scissor);

    // Bind pipeline
    vkCommandBuffers_[currentFrame_].bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline_.get());

    // Draw
    vkCommandBuffers_[currentFrame_].draw(3, 1, 0, 0);

    // End
    vkCommandBuffers_[currentFrame_].endRenderPass();
    vkCommandBuffers_[currentFrame_].end();

    // Submit command buffer to queue
    vk::PipelineStageFlags waitDestinationStageMask{vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SubmitInfo submitInfo{*imageAcquiredSemaphores_[currentFrame_],
                              waitDestinationStageMask,
                              *vkCommandBuffers_[currentFrame_],
                              *renderFinishedSemaphores_[currentFrame_]};
    device_.get().resetFences({*drawFences_[currentFrame_]});
    device_.getGraphicQueue().submit(submitInfo, *drawFences_[currentFrame_]);

    // Wait for submit
    while (vk::Result::eTimeout ==
           device_.get().waitForFences({*drawFences_[currentFrame_]}, VK_TRUE, std::numeric_limits<uint64_t>::max()))
        ;

    // Submit acquired image to present queue
    vk::PresentInfoKHR presentInfo{*renderFinishedSemaphores_[currentFrame_], *pSwapChain_->get(), imageIndex};
    result = device_.getPresentQueue().presentKHR(presentInfo);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
        window_.isFramebufferResized()) {
        recreateSwapChain();
        window_.resetFramebufferResized();
        return;
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Graphic::recreateSwapChain() {
    auto windowExtent = window_.getExtent();
    while (windowExtent.width == 0 || windowExtent.height == 0) {
        windowExtent = window_.getExtent();
        glfwWrapper([]() { glfwWaitEvents(); });
    }

    device_.get().waitIdle();

    pSwapChain_ = std::make_unique<graphic::SwapChain>(
            physicalDevice_,
            surface_,
            device_,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc);
    pSwapChain_->createFrameBuffers(device_, pipeline_.getRenderPass());
}

} // namespace nae
