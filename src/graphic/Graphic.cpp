#include "graphic/Graphic.hpp"

#include <chrono>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "GlfwApi.hpp"
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
      descriptorPool_{device_, {{vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT}}},
      descriptorSetLayout_{device_, {{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}}},
      pipeline_{device_, *pSwapChain_, descriptorSetLayout_, util::vertShaderPath, util::fragShaderPath},
      vkCommandPool_{device_.get(),
                     vk::CommandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                               device_.getGraphicQueueFamilyIndex()}},
      vkCommandBuffers_{
              device_.get(),
              vk::CommandBufferAllocateInfo{*vkCommandPool_, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT}} {
    pSwapChain_->createFrameBuffers(device_, pipeline_.getRenderPass());

    mvpBuffers_.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        mvpBuffers_.emplace_back(device_, sizeof(graphic::MvpMatrices), vk::BufferUsageFlagBits::eUniformBuffer);
        mvpBuffers_[i].mapMemory();
    }

    std::vector<vk::DescriptorSetLayout> vkDescriptorSetLayouts(2, *descriptorSetLayout_.get());
    pDescriptorSets_ = std::make_unique<graphic::DescriptorSets>(device_, descriptorPool_, vkDescriptorSetLayouts);
    pDescriptorSets_->update(mvpBuffers_);

    const std::vector<graphic::Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                                   {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                                   {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                                   {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
    pVertexBuffer_ = std::make_unique<graphic::Buffer>(
            device_,
            sizeof(graphic::Vertex) * vertices.size(),
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal);
    pVertexBuffer_->copyToDeviceMemory(vkCommandPool_, device_.getGraphicQueue(), vertices);

    // TODO: unify index buffer and vertex buffer
    const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
    pIndexBuffer_ = std::make_unique<graphic::Buffer>(
            device_,
            sizeof(uint16_t) * indices.size(),
            vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal);
    pIndexBuffer_->copyToDeviceMemory(vkCommandPool_, device_.getGraphicQueue(), indices);

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
    vk::Result result{};
    uint32_t imageIndex = 0;
    std::tie(result, imageIndex) = pSwapChain_->get().acquireNextImage(std::numeric_limits<uint64_t>::max(),
                                                                       *imageAcquiredSemaphores_[currentFrame_]);
    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return;
    }
    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Update mvp matrices
    updateMvpMatrix(currentFrame_);

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

    // Bind vertex and index buffer
    vkCommandBuffers_[currentFrame_].bindVertexBuffers(0, {*pVertexBuffer_->get()}, {0});
    vkCommandBuffers_[currentFrame_].bindIndexBuffer(*pIndexBuffer_->get(), 0, vk::IndexType::eUint16);

    // Bind pipeline
    vkCommandBuffers_[currentFrame_].bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline_.get());

    // Bind descriptor set
    vkCommandBuffers_[currentFrame_].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                        *pipeline_.getLayout(),
                                                        0,
                                                        {*(*pDescriptorSets_)[currentFrame_]},
                                                        {});

    // Draw
    vkCommandBuffers_[currentFrame_].drawIndexed(static_cast<uint32_t>(6), 1, 0, 0, 0);

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
           device_.get().waitForFences({*drawFences_[currentFrame_]}, VK_TRUE, std::numeric_limits<uint64_t>::max())) {}

    // Submit acquired image to present queue
    vk::PresentInfoKHR presentInfo{*renderFinishedSemaphores_[currentFrame_], *pSwapChain_->get(), imageIndex};
    result = device_.getPresentQueue().presentKHR(presentInfo);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
        window_.get().isFramebufferResized()) {
        recreateSwapChain();
        window_.get().resetFramebufferResized();
        return;
    }
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Graphic::recreateSwapChain() {
    auto windowExtent = window_.get().getExtent();
    while (windowExtent.width == 0 || windowExtent.height == 0) {
        windowExtent = window_.get().getExtent();
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

void Graphic::updateMvpMatrix(uint32_t currentFrame) const {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    graphic::MvpMatrices mvpMatrices{
            .model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3{0.0f, 0.0f, 1.0f}),
            .view = glm::lookAt(glm::vec3{2.0f, 2.0f, 2.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}),
            .proj = glm::perspective(glm::radians(45.0f),
                                     static_cast<float>(pSwapChain_->getExtent().width) /
                                             static_cast<float>(pSwapChain_->getExtent().height),
                                     0.1f,
                                     10.0f)};
    mvpMatrices.proj[1][1] *= -1;
    mvpBuffers_[currentFrame].writeToMemory(mvpMatrices);
    mvpBuffers_[currentFrame].flushMemory();
}

} // namespace nae
