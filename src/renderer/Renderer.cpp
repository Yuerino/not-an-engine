#include "renderer/Renderer.hpp"

#include <limits>

#include "core/App.hpp"
#include "scene/Model.hpp"

namespace nae {

Renderer::Renderer() {
    const auto &graphicContext = App::get().getGraphicContext();
    const auto &device = graphicContext.getDevice();

    pRenderPass_ = std::make_unique<RenderPass>(graphicContext.getSwapchain().getFormat().format,
                                                vk::Format::eD32Sfloat);

    pDescriptorPool_ = std::make_unique<DescriptorPool>(
            device,
            std::initializer_list<vk::DescriptorPoolSize>{{vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT},
                                                          {vk::DescriptorType::eCombinedImageSampler, 10}});

    pVkCommandPool_ = std::make_unique<vk::raii::CommandPool>(
            device.get(),
            vk::CommandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                      device.getGraphicQueueFamilyIndex()});
    pVkCommandBuffers_ = std::make_unique<vk::raii::CommandBuffers>(
            device.get(),
            vk::CommandBufferAllocateInfo{**pVkCommandPool_, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT});

    // TODO: check max anisotropy filter sampler
    pVkSampler_ = std::make_unique<vk::raii::Sampler>(device.get(),
                                                      vk::SamplerCreateInfo{vk::SamplerCreateFlags{},
                                                                            vk::Filter::eLinear,
                                                                            vk::Filter::eLinear,
                                                                            vk::SamplerMipmapMode::eLinear,
                                                                            vk::SamplerAddressMode::eRepeat,
                                                                            vk::SamplerAddressMode::eRepeat,
                                                                            vk::SamplerAddressMode::eRepeat,
                                                                            0.0f,
                                                                            true,
                                                                            16.0f,
                                                                            false,
                                                                            vk::CompareOp::eAlways,
                                                                            0.0f,
                                                                            0.0f,
                                                                            vk::BorderColor::eFloatOpaqueWhite,
                                                                            false});

    imageAcquiredSemaphores_.reserve(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores_.reserve(MAX_FRAMES_IN_FLIGHT);
    drawFences_.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        imageAcquiredSemaphores_.emplace_back(device.get(), vk::SemaphoreCreateInfo{});
        renderFinishedSemaphores_.emplace_back(device.get(), vk::SemaphoreCreateInfo{});
        drawFences_.emplace_back(device.get(), vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
    }
}

bool Renderer::onRender() {
    if (!beginFrame()) {
        return false;
    }

    startRenderPass();

    renderModels();

    endRenderPass();

    if (!endFrame()) {
        return false;
    }

    return true;
}

bool Renderer::beginFrame() {
    const auto &swapchain = App::get().getGraphicContext().getSwapchain();

    vk::Result result{};
    std::tie(result, currentFrameBufferIdx_) = swapchain.get().acquireNextImage(
            std::numeric_limits<uint64_t>::max(),
            *imageAcquiredSemaphores_[currentCommandBufferIdx_]);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        return false;
    }

    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    return true;
}

void Renderer::startRenderPass() {
    const auto &swapchain = App::get().getGraphicContext().getSwapchain();

    // Begin command buffer
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].reset(vk::CommandBufferResetFlags{});
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].begin(vk::CommandBufferBeginInfo{});

    // Render pass begin
    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0] = vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1] = vk::ClearDepthStencilValue{1.0f, 0};
    vk::RenderPassBeginInfo renderPassBeginInfo{*pRenderPass_->get(),
                                                *swapchain.getFrameBuffers()[currentFrameBufferIdx_],
                                                vk::Rect2D{vk::Offset2D{0, 0}, swapchain.getExtent()},
                                                clearValues};
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    // Clear descriptor sets to bind
    //    descriptorSetsToBind_.clear();

    // Bind dynamic view port and scissor
    vk::Viewport viewPort{0.0f,
                          0.0f,
                          static_cast<float>(swapchain.getExtent().width),
                          static_cast<float>(swapchain.getExtent().height),
                          0.0f,
                          1.0f};
    vk::Rect2D scissor{vk::Offset2D{0, 0}, swapchain.getExtent()};
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].setViewport(0, viewPort);
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].setScissor(0, scissor);
}

void Renderer::endRenderPass() {
    const auto &device = App::get().getGraphicContext().getDevice();

    (*pVkCommandBuffers_)[currentCommandBufferIdx_].endRenderPass();
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].end();

    // Submit command buffer to queue
    vk::PipelineStageFlags waitDestinationStageMask{vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SubmitInfo submitInfo{*imageAcquiredSemaphores_[currentCommandBufferIdx_],
                              waitDestinationStageMask,
                              *(*pVkCommandBuffers_)[currentCommandBufferIdx_],
                              *renderFinishedSemaphores_[currentCommandBufferIdx_]};
    device.get().resetFences({*drawFences_[currentCommandBufferIdx_]});
    device.getGraphicQueue().submit(submitInfo, *drawFences_[currentCommandBufferIdx_]);

    // Wait for submit
    while (vk::Result::eTimeout == device.get().waitForFences({*drawFences_[currentCommandBufferIdx_]},
                                                              VK_TRUE,
                                                              std::numeric_limits<uint64_t>::max())) {}
}

bool Renderer::endFrame() {
    const auto &device = App::get().getGraphicContext().getDevice();
    const auto &swapchain = App::get().getGraphicContext().getSwapchain();

    // Submit acquired image to present queue
    vk::Result result{};
    vk::PresentInfoKHR presentInfo{*renderFinishedSemaphores_[currentCommandBufferIdx_],
                                   *swapchain.get(),
                                   currentFrameBufferIdx_};
    result = device.getPresentQueue().presentKHR(presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        return false;
    }

    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentCommandBufferIdx_ = (currentCommandBufferIdx_ + 1) % MAX_FRAMES_IN_FLIGHT;
    return true;
}

void Renderer::renderModels() {
    auto &scene = App::get().getActiveScene();

    // Bind camera
    auto &camera = scene.getCamera();
    camera.bind((*pVkCommandBuffers_)[currentCommandBufferIdx_]);

    auto models = scene.getEntityManager().queryComponentOfType(EComponentType::Model);
    std::for_each(models.begin(), models.end(), [&](auto *pComponent) -> void {
        auto pModel = static_cast<Model *>(pComponent);
        pModel->draw((*pVkCommandBuffers_)[currentCommandBufferIdx_]);
    });
}

} // namespace nae
