#include "renderer/Renderer.hpp"

#include <chrono>
#include <limits>

#include <glm/gtc/matrix_transform.hpp>

#include "core/util.hpp"

namespace nae {

Renderer::Renderer(const GraphicInstance &graphicInstance)
    : pGraphicInstance_{&graphicInstance},
      descriptorPool_{graphicInstance.getDevice(), {{vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT}}},
      descriptorSetLayout_{graphicInstance.getDevice(),
                           {{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}}},
      pipeline_{graphicInstance.getDevice(),
                graphicInstance.getSwapchain(),
                descriptorSetLayout_,
                util::vertShaderPath,
                util::fragShaderPath},
      vkCommandPool_{graphicInstance.getDevice().get(),
                     vk::CommandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                               graphicInstance.getDevice().getGraphicQueueFamilyIndex()}},
      vkCommandBuffers_{
              graphicInstance.getDevice().get(),
              vk::CommandBufferAllocateInfo{*vkCommandPool_, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT}} {

    mvpBuffers_.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        mvpBuffers_.emplace_back(graphicInstance.getDevice(),
                                 sizeof(MvpMatrices),
                                 vk::BufferUsageFlagBits::eUniformBuffer);
        mvpBuffers_[i].mapMemory();
    }

    std::vector<vk::DescriptorSetLayout> vkDescriptorSetLayouts(MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout_.get());
    pDescriptorSets_ =
            std::make_unique<DescriptorSets>(graphicInstance.getDevice(), descriptorPool_, vkDescriptorSetLayouts);
    pDescriptorSets_->update(mvpBuffers_);

    imageAcquiredSemaphores_.reserve(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores_.reserve(MAX_FRAMES_IN_FLIGHT);
    drawFences_.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        imageAcquiredSemaphores_.emplace_back(pGraphicInstance_->getDevice().get(), vk::SemaphoreCreateInfo{});
        renderFinishedSemaphores_.emplace_back(graphicInstance.getDevice().get(), vk::SemaphoreCreateInfo{});
        drawFences_.emplace_back(graphicInstance.getDevice().get(),
                                 vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
    }
}

bool Renderer::beginFrame() {
    vk::Result result{};
    std::tie(result, currentFrameBufferIdx_) = pGraphicInstance_->getSwapchain().get().acquireNextImage(
            std::numeric_limits<uint64_t>::max(),
            *imageAcquiredSemaphores_[currentCommandBufferIdx_]);
    if (result == vk::Result::eErrorOutOfDateKHR) {
        return false;
    }
    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Begin command buffer
    vkCommandBuffers_[currentCommandBufferIdx_].reset(vk::CommandBufferResetFlags{});
    vkCommandBuffers_[currentCommandBufferIdx_].begin(vk::CommandBufferBeginInfo{});

    // Render pass begin
    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0] = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1] = vk::ClearDepthStencilValue{1.0f, 0};
    vk::RenderPassBeginInfo renderPassBeginInfo{
            *pipeline_.getRenderPass().get(),
            *pGraphicInstance_->getSwapchain().getFrameBuffers()[currentFrameBufferIdx_],
            vk::Rect2D{vk::Offset2D{0, 0}, pGraphicInstance_->getSwapchain().getExtent()},
            clearValues};
    vkCommandBuffers_[currentCommandBufferIdx_].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    return true;
}

void Renderer::beginScene() {
    {
        // TODO: move this mvp outside to scene scope
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        MvpMatrices mvpMatrices{
                //            .model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3{0.0f, 1.0f,
                //            0.0f}),
                .model = glm::mat4(1.0f),
                .view = glm::lookAt(glm::vec3{0.0f, 0.0f, 5.0f},
                                    glm::vec3{0.0f, 0.0f, 0.0f},
                                    glm::vec3{0.0f, 1.0f, 0.0f}),
                .proj = glm::perspective(
                        glm::radians(45.0f),
                        static_cast<float>(pGraphicInstance_->getSwapchain().getExtent().width) /
                                static_cast<float>(pGraphicInstance_->getSwapchain().getExtent().height),
                        0.1f,
                        100.0f)};
        mvpMatrices.proj[1][1] *= -1.0f;
        mvpBuffers_[currentCommandBufferIdx_].writeToMemory(mvpMatrices);
        mvpBuffers_[currentCommandBufferIdx_].flushMemory();
    }

    // Bind dynamic view port and scissor
    vk::Viewport viewPort{0.0f,
                          0.0f,
                          static_cast<float>(pGraphicInstance_->getSwapchain().getExtent().width),
                          static_cast<float>(pGraphicInstance_->getSwapchain().getExtent().height),
                          0.0f,
                          1.0f};
    vk::Rect2D scissor{vk::Offset2D{0, 0}, pGraphicInstance_->getSwapchain().getExtent()};
    vkCommandBuffers_[currentCommandBufferIdx_].setViewport(0, viewPort);
    vkCommandBuffers_[currentCommandBufferIdx_].setScissor(0, scissor);

    // Bind pipeline
    vkCommandBuffers_[currentCommandBufferIdx_].bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline_.get());

    // Bind descriptor set
    vkCommandBuffers_[currentCommandBufferIdx_].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                                   *pipeline_.getLayout(),
                                                                   0,
                                                                   {*(*pDescriptorSets_)[currentCommandBufferIdx_]},
                                                                   {});
}

void Renderer::endScene() {
    vkCommandBuffers_[currentCommandBufferIdx_].endRenderPass();
    vkCommandBuffers_[currentCommandBufferIdx_].end();

    // Submit command buffer to queue
    vk::PipelineStageFlags waitDestinationStageMask{vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SubmitInfo submitInfo{*imageAcquiredSemaphores_[currentCommandBufferIdx_],
                              waitDestinationStageMask,
                              *vkCommandBuffers_[currentCommandBufferIdx_],
                              *renderFinishedSemaphores_[currentCommandBufferIdx_]};
    pGraphicInstance_->getDevice().get().resetFences({*drawFences_[currentCommandBufferIdx_]});
    pGraphicInstance_->getDevice().getGraphicQueue().submit(submitInfo, *drawFences_[currentCommandBufferIdx_]);

    // Wait for submit
    while (vk::Result::eTimeout ==
           pGraphicInstance_->getDevice().get().waitForFences({*drawFences_[currentCommandBufferIdx_]},
                                                              VK_TRUE,
                                                              std::numeric_limits<uint64_t>::max())) {}
}

Buffer Renderer::loadVertices(const std::vector<Vertex> &vertices) {
    Buffer verticesBuffer = {pGraphicInstance_->getDevice(),
                             sizeof(Vertex) * vertices.size(),
                             vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                             vk::MemoryPropertyFlagBits::eDeviceLocal};
    verticesBuffer.copyToDeviceMemory(vkCommandPool_, pGraphicInstance_->getDevice().getGraphicQueue(), vertices);
    return verticesBuffer;
}

bool Renderer::endFrame() {
    // Submit acquired image to present queue
    vk::Result result{};
    vk::PresentInfoKHR presentInfo{*renderFinishedSemaphores_[currentCommandBufferIdx_],
                                   *pGraphicInstance_->getSwapchain().get(),
                                   currentFrameBufferIdx_};
    result = pGraphicInstance_->getDevice().getPresentQueue().presentKHR(presentInfo);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        return false;
    }
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentCommandBufferIdx_ = (currentCommandBufferIdx_ + 1) % MAX_FRAMES_IN_FLIGHT;
    return true;
}

void Renderer::draw(const Buffer &buffer, uint32_t vertexCount) {
    vkCommandBuffers_[currentCommandBufferIdx_].bindVertexBuffers(0, {*buffer.get()}, {0});
    vkCommandBuffers_[currentCommandBufferIdx_].draw(vertexCount, 1, 0, 0);
}

const Pipeline &Renderer::getPipeline() const noexcept {
    return pipeline_;
}

} // namespace nae
