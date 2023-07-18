#include "graphic/Graphic.hpp"

#include <iostream>
#include <thread>

#include "graphic/CommandBuffer.hpp"
#include "graphic/Descriptor.hpp"
#include "graphic/Device.hpp"
#include "graphic/FrameBuffer.hpp"
#include "graphic/PhysicalDevice.hpp"
#include "graphic/Pipeline.hpp"
#include "graphic/RenderPass.hpp"
#include "graphic/Shader.hpp"
#include "graphic/Surface.hpp"
#include "util.hpp"

namespace nae {

Graphic::Graphic(const Window &window)
    : window_(window), instance_{{}, graphic::Instance::getGlfwRequiredExtensions()} {
    vkPhysicalDevicePtr_ = std::make_unique<vk::raii::PhysicalDevice>(graphic::createPhysicalDevice(*instance_));

    vkSurfacePtr_ = std::make_unique<vk::raii::SurfaceKHR>(graphic::createSurface(*instance_, window_.glfwWindow_));

    std::tie(graphicQueueFamilyIndex_, presentQueueFamilyIndex_) =
            graphic::findGraphicAndPresentQueueFamilyIndex(*vkPhysicalDevicePtr_, *vkSurfacePtr_);
    vkDevicePtr_ = std::make_unique<vk::raii::Device>(
            graphic::createDevice(*vkPhysicalDevicePtr_, graphicQueueFamilyIndex_, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}));

    vkCommandPool_ = vk::raii::CommandPool{
            *vkDevicePtr_,
            vk::CommandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphicQueueFamilyIndex_}};
    vkCommandBuffer_ = graphic::createCommandBuffer(*vkDevicePtr_, vkCommandPool_);

    vkGraphicQueue_ = vk::raii::Queue{*vkDevicePtr_, graphicQueueFamilyIndex_, 0};
    vkPresentQueue_ = vk::raii::Queue{*vkDevicePtr_, presentQueueFamilyIndex_, 0};

    SwapChainDataPtr_ = std::make_unique<graphic::SwapChainData>(
            *vkPhysicalDevicePtr_,
            *vkSurfacePtr_,
            *vkDevicePtr_,
            window_.extent_,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
            graphicQueueFamilyIndex_,
            presentQueueFamilyIndex_);

    depthBufferData_ =
            graphic::DepthBufferData{*vkPhysicalDevicePtr_, *vkDevicePtr_, vk::Format::eD16Unorm, window_.extent_};

    uniformBufferData_ = graphic::BufferData(*vkPhysicalDevicePtr_,
                                             *vkDevicePtr_,
                                             sizeof(glm::mat4x4),
                                             vk::BufferUsageFlagBits::eUniformBuffer);

    modelMatrix_ = util::createModelViewProjectionClipMatrix(window_.extent_);
    graphic::copyToDevice(uniformBufferData_.deviceMemory_, modelMatrix_);

    vkDescriptorSetLayout_ = graphic::createDescriptorSetLayout(
            *vkDevicePtr_,
            {{vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}});
    vkPipelineLayout_ = vk::raii::PipelineLayout{*vkDevicePtr_, {{}, *vkDescriptorSetLayout_}};

    vkRenderPass_ = graphic::createRenderPass(
            *vkDevicePtr_,
            graphic::pickSurfaceFormat(vkPhysicalDevicePtr_->getSurfaceFormatsKHR(**vkSurfacePtr_)).format,
            depthBufferData_.format_);

    frameBuffers_ = graphic::createFrameBuffers(*vkDevicePtr_,
                                                vkRenderPass_,
                                                SwapChainDataPtr_->imageViews_,
                                                &depthBufferData_.imageView_,
                                                window_.extent_);

    vertexBufferData_ = graphic::BufferData(*vkPhysicalDevicePtr_,
                                            *vkDevicePtr_,
                                            sizeof(util::coloredCubeData),
                                            vk::BufferUsageFlagBits::eVertexBuffer);
    graphic::copyToDevice(vertexBufferData_.deviceMemory_,
                          util::coloredCubeData,
                          sizeof(util::coloredCubeData) / sizeof(util::coloredCubeData[0]));

    vkDescriptorPool_ = graphic::createDescriptorPool(*vkDevicePtr_, {{vk::DescriptorType::eUniformBuffer, 1}});
    vkDescriptorSet_ = std::move(
            vk::raii::DescriptorSets(*vkDevicePtr_, {*vkDescriptorPool_, *vkDescriptorSetLayout_}).front());
    graphic::updateDescriptorSets(
            *vkDevicePtr_,
            vkDescriptorSet_,
            {{vk::DescriptorType::eUniformBuffer, uniformBufferData_.buffer_, VK_WHOLE_SIZE, nullptr}},
            {});

    vkVertexShaderModule_ = graphic::createShaderModule(
            *vkDevicePtr_,
            vk::ShaderStageFlagBits::eVertex,
            "C:/Users/yueri/Documents/Project/not-an-engine/shader/shader.vert.spv");
    vkFragmentShaderModule_ = graphic::createShaderModule(
            *vkDevicePtr_,
            vk::ShaderStageFlagBits::eFragment,
            "C:/Users/yueri/Documents/Project/not-an-engine/shader/shader.frag.spv");

    vkPipelineCache_ = vk::raii::PipelineCache{*vkDevicePtr_, vk::PipelineCacheCreateInfo{}};
    graphicPipeline_ = graphic::createGraphicPipeline(
            *vkDevicePtr_,
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
    vk::raii::Semaphore imageAcquiredSemaphore{*vkDevicePtr_, vk::SemaphoreCreateInfo{}};

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

    vk::raii::Fence drawFence{*vkDevicePtr_, vk::FenceCreateInfo{}};

    vk::PipelineStageFlags waitDestinationStageMask{vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SubmitInfo submitInfo{*imageAcquiredSemaphore, waitDestinationStageMask, *vkCommandBuffer_};
    vkGraphicQueue_.submit(submitInfo, *drawFence);

    while (vk::Result::eTimeout == vkDevicePtr_->waitForFences({*drawFence}, VK_TRUE, 100000000))
        ;

    vk::PresentInfoKHR presentInfoKHR{nullptr, *SwapChainDataPtr_->vkSwapChain_, imageIndex};
    result = vkPresentQueue_.presentKHR(presentInfoKHR);
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

    vkDevicePtr_->waitIdle();
}

} // namespace nae
