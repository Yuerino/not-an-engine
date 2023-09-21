#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "core/Time.hpp"
#include "renderer/Descriptor.hpp"
#include "renderer/Pipeline.hpp"

namespace nae {

class Renderer {
public:
    Renderer();

    bool onRender();

    [[nodiscard]] const RenderPass &getRenderPass() const noexcept { return *pRenderPass_; };
    [[nodiscard]] const DescriptorPool &getDescriptorPool() const noexcept { return *pDescriptorPool_; };
    [[nodiscard]] const vk::raii::CommandPool &getVkCommandPool() const noexcept { return *pVkCommandPool_; };
    [[nodiscard]] const vk::raii::Sampler &getVkSampler() const noexcept { return *pVkSampler_; };
    // TODO: managing class for descriptor sets
    [[nodiscard]] std::vector<vk::DescriptorSet> &getDescriptorSetsToBind() noexcept { return descriptorSetsToBind_; };
    [[nodiscard]] static constexpr uint32_t getMaxFramesInFlight() noexcept { return MAX_FRAMES_IN_FLIGHT; };
    [[nodiscard]] uint32_t getCurrentCommandBufferIdx() const noexcept { return currentCommandBufferIdx_; };

private:
    bool beginFrame();
    bool endFrame();

    void startRenderPass();
    void endRenderPass();

    void renderModels();

private:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    std::unique_ptr<RenderPass> pRenderPass_;
    std::unique_ptr<DescriptorPool> pDescriptorPool_;
    std::unique_ptr<vk::raii::CommandPool> pVkCommandPool_;
    std::unique_ptr<vk::raii::CommandBuffers> pVkCommandBuffers_;
    std::unique_ptr<vk::raii::Sampler> pVkSampler_;
    std::vector<vk::DescriptorSet> descriptorSetsToBind_;

    std::vector<vk::raii::Semaphore> imageAcquiredSemaphores_;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores_;
    std::vector<vk::raii::Fence> drawFences_;

    uint32_t currentFrameBufferIdx_{0};
    uint32_t currentCommandBufferIdx_{0};
};

} // namespace nae
