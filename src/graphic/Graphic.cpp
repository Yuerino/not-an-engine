#include "graphic/Graphic.hpp"

#include <iostream>
#include <thread>

#include "graphic/CommandBuffer.hpp"
#include "graphic/Descriptor.hpp"
#include "graphic/FrameBuffer.hpp"
#include "graphic/Pipeline.hpp"
#include "graphic/RenderPass.hpp"
#include "graphic/Shader.hpp"
#include "util.hpp"

namespace nae {

Graphic::Graphic(const Window &window)
    : window_(window),
      instance_{{}, graphic::Instance::getGlfwRequiredExtensions()},
      surface_{instance_, window_},
      physicalDevice_{instance_, surface_, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}},
      device_{physicalDevice_, surface_} {
    vkCommandPool_ = vk::raii::CommandPool{device_.get(),
                                           vk::CommandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                                                     device_.getGraphicQueueFamilyIndex()}};
    vkCommandBuffer_ = graphic::createCommandBuffer(device_.get(), vkCommandPool_);

    SwapChainDataPtr_ = std::make_unique<graphic::SwapChainData>(
            physicalDevice_,
            surface_,
            device_.get(),
            window_.extent_,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
            device_.getGraphicQueueFamilyIndex(),
            device_.getPresentQueueFamilyIndex());

    depthBufferData_ = graphic::DepthBufferData{device_, vk::Format::eD16Unorm, window_.extent_};

    uniformBufferData_ = graphic::BufferData(device_, sizeof(glm::mat4x4), vk::BufferUsageFlagBits::eUniformBuffer);

    modelMatrix_ = util::createModelViewProjectionClipMatrix(window_.extent_);
    graphic::copyToDevice(uniformBufferData_.deviceMemory_, modelMatrix_);

    vkDescriptorSetLayout_ = graphic::createDescriptorSetLayout(
            device_.get(),
            {{vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}});
    vkPipelineLayout_ = vk::raii::PipelineLayout{device_.get(), {{}, *vkDescriptorSetLayout_}};

    vkRenderPass_ = graphic::createRenderPass(device_.get(),
                                              surface_.pickSurfaceFormat(physicalDevice_).format,
                                              depthBufferData_.format_);

    frameBuffers_ = graphic::createFrameBuffers(device_.get(),
                                                vkRenderPass_,
                                                SwapChainDataPtr_->imageViews_,
                                                &depthBufferData_.imageView_,
                                                window_.extent_);

    vertexBufferData_ =
            graphic::BufferData(device_, sizeof(util::coloredCubeData), vk::BufferUsageFlagBits::eVertexBuffer);
    graphic::copyToDevice(vertexBufferData_.deviceMemory_,
                          util::coloredCubeData,
                          sizeof(util::coloredCubeData) / sizeof(util::coloredCubeData[0]));

    vkDescriptorPool_ = graphic::createDescriptorPool(device_.get(), {{vk::DescriptorType::eUniformBuffer, 1}});
    vkDescriptorSet_ = std::move(
            vk::raii::DescriptorSets(device_.get(), {*vkDescriptorPool_, *vkDescriptorSetLayout_}).front());
    graphic::updateDescriptorSets(
            device_.get(),
            vkDescriptorSet_,
            {{vk::DescriptorType::eUniformBuffer, uniformBufferData_.buffer_, VK_WHOLE_SIZE, nullptr}},
            {});

    vkVertexShaderModule_ =
            graphic::createShaderModule(device_.get(), vk::ShaderStageFlagBits::eVertex, util::vertShaderPath);
    vkFragmentShaderModule_ =
            graphic::createShaderModule(device_.get(), vk::ShaderStageFlagBits::eFragment, util::fragShaderPath);

    vkPipelineCache_ = vk::raii::PipelineCache{device_.get(), vk::PipelineCacheCreateInfo{}};
    graphicPipeline_ = graphic::createGraphicPipeline(
            device_.get(),
            vkPipelineCache_,
            vkVertexShaderModule_,
            nullptr,
            vkFragmentShaderModule_,
            nullptr,
            util::checked_cast<uint32_t>(sizeof(util::coloredCubeData[0])),
            {{vk::Format::eR32G32B32A32Sfloat, 0}, {vk::Format::eR32G32B32A32Sfloat, 16}},
            vk::FrontFace::eClockwise,
            true,
            vkPipelineLayout_,
            vkRenderPass_);
}

void Graphic::Update() {
    vk::raii::Semaphore imageAcquiredSemaphore{device_.get(), vk::SemaphoreCreateInfo{}};

    vk::Result result;
    uint32_t imageIndex;
    std::tie(result, imageIndex) = SwapChainDataPtr_->vkSwapChain_.acquireNextImage(100000000, *imageAcquiredSemaphore);
    assert(result == vk::Result::eSuccess);
    assert(imageIndex < SwapChainDataPtr_->images_.size());

    vkCommandBuffer_.begin({});

    std::array<vk::ClearValue, 2> clearValues;
    clearValues[0].color = vk::ClearColorValue{0.2f, 0.2f, 0.2f, 0.2f};
    clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};
    vk::RenderPassBeginInfo renderPassBeginInfo{*vkRenderPass_,
                                                *frameBuffers_[imageIndex],
                                                vk::Rect2D{vk::Offset2D{0, 0}, window_.extent_},
                                                clearValues};
    vkCommandBuffer_.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    vkCommandBuffer_.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicPipeline_);
    vkCommandBuffer_.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                        *vkPipelineLayout_,
                                        0,
                                        {*vkDescriptorSet_},
                                        nullptr);

    vkCommandBuffer_.bindVertexBuffers(0, {*vertexBufferData_.buffer_}, {0});
    vkCommandBuffer_.setViewport(0,
                                 vk::Viewport{0.0f,
                                              0.0f,
                                              static_cast<float>(window_.extent_.width),
                                              static_cast<float>(window_.extent_.height),
                                              0.0f,
                                              1.0f});
    vkCommandBuffer_.setScissor(0, vk::Rect2D{vk::Offset2D{0, 0}, window_.extent_});

    vkCommandBuffer_.draw(12 * 3, 1, 0, 0);
    vkCommandBuffer_.endRenderPass();
    vkCommandBuffer_.end();

    vk::raii::Fence drawFence{device_.get(), vk::FenceCreateInfo{}};

    vk::PipelineStageFlags waitDestinationStageMask{vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SubmitInfo submitInfo{*imageAcquiredSemaphore, waitDestinationStageMask, *vkCommandBuffer_};
    device_.getGraphicQueue().submit(submitInfo, *drawFence);

    while (vk::Result::eTimeout == device_.get().waitForFences({*drawFence}, VK_TRUE, 100000000))
        ;

    vk::PresentInfoKHR presentInfoKHR{nullptr, *SwapChainDataPtr_->vkSwapChain_, imageIndex};
    result = device_.getPresentQueue().presentKHR(presentInfoKHR);
    switch (result) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
            std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
            break;
        default:
            assert(false); // an unexpected result is returned !
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

Graphic::~Graphic() {
    device_.get().waitIdle();
}

} // namespace nae
