#include "renderer/Renderer.hpp"

#include <chrono>
#include <limits>

#include <glm/gtc/matrix_transform.hpp>

#include "core/App.hpp"
#include "core/util.hpp"

namespace nae {

Renderer::Renderer() {
    const auto &graphicContext = App::get().getGraphicContext();
    const auto &device = graphicContext.getDevice();

    pDescriptorPool_ = std::make_unique<DescriptorPool>(
            device,
            std::initializer_list<vk::DescriptorPoolSize>{{vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT}});
    pDescriptorSetLayout_ = std::make_unique<DescriptorSetLayout>(
            device,
            std::initializer_list<vk::DescriptorSetLayoutBinding>{
                    {0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}});

    pPipeline_ = std::make_unique<Pipeline>(device,
                                            graphicContext.getSwapchain(),
                                            *pDescriptorSetLayout_,
                                            util::vertShaderPath,
                                            util::fragShaderPath);

    pVkCommandPool_ = std::make_unique<vk::raii::CommandPool>(
            device.get(),
            vk::CommandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                      device.getGraphicQueueFamilyIndex()});
    pVkCommandBuffers_ = std::make_unique<vk::raii::CommandBuffers>(
            device.get(),
            vk::CommandBufferAllocateInfo{**pVkCommandPool_, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT});

    mvpBuffers_.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        mvpBuffers_.emplace_back(device, sizeof(MvpMatrices), vk::BufferUsageFlagBits::eUniformBuffer);
        mvpBuffers_[i].mapMemory();
    }

    std::vector<vk::DescriptorSetLayout> vkDescriptorSetLayouts(MAX_FRAMES_IN_FLIGHT, *pDescriptorSetLayout_->get());
    pDescriptorSets_ = std::make_unique<DescriptorSets>(device, *pDescriptorPool_, vkDescriptorSetLayouts);
    pDescriptorSets_->update(mvpBuffers_);

    imageAcquiredSemaphores_.reserve(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores_.reserve(MAX_FRAMES_IN_FLIGHT);
    drawFences_.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        imageAcquiredSemaphores_.emplace_back(device.get(), vk::SemaphoreCreateInfo{});
        renderFinishedSemaphores_.emplace_back(device.get(), vk::SemaphoreCreateInfo{});
        drawFences_.emplace_back(device.get(), vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
    }
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

    // Begin command buffer
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].reset(vk::CommandBufferResetFlags{});
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].begin(vk::CommandBufferBeginInfo{});

    // Render pass begin
    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0] = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1] = vk::ClearDepthStencilValue{1.0f, 0};
    vk::RenderPassBeginInfo renderPassBeginInfo{*pPipeline_->getRenderPass().get(),
                                                *swapchain.getFrameBuffers()[currentFrameBufferIdx_],
                                                vk::Rect2D{vk::Offset2D{0, 0}, swapchain.getExtent()},
                                                clearValues};
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    return true;
}

void Renderer::beginScene() {
    const auto &swapchain = App::get().getGraphicContext().getSwapchain();

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
                .proj = glm::perspective(glm::radians(45.0f),
                                         static_cast<float>(swapchain.getExtent().width) /
                                                 static_cast<float>(swapchain.getExtent().height),
                                         0.1f,
                                         100.0f)};
        mvpMatrices.proj[1][1] *= -1.0f;
        mvpBuffers_[currentCommandBufferIdx_].writeToMemory(mvpMatrices);
        mvpBuffers_[currentCommandBufferIdx_].flushMemory();
    }

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

    // Bind pipeline
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].bindPipeline(vk::PipelineBindPoint::eGraphics, *pPipeline_->get());

    // Bind descriptor set
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                                       *pPipeline_->getLayout(),
                                                                       0,
                                                                       {*(*pDescriptorSets_)[currentCommandBufferIdx_]},
                                                                       {});
}

void Renderer::endScene() {
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

Buffer Renderer::loadVertices(const std::vector<Vertex> &vertices) {
    const auto &device = App::get().getGraphicContext().getDevice();

    Buffer verticesBuffer = {device,
                             sizeof(Vertex) * vertices.size(),
                             vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                             vk::MemoryPropertyFlagBits::eDeviceLocal};
    verticesBuffer.copyToDeviceMemory(*pVkCommandPool_, device.getGraphicQueue(), vertices);
    return verticesBuffer;
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

void Renderer::draw(const Buffer &buffer, uint32_t vertexCount) {
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].bindVertexBuffers(0, {*buffer.get()}, {0});
    (*pVkCommandBuffers_)[currentCommandBufferIdx_].draw(vertexCount, 1, 0, 0);
}

} // namespace nae
