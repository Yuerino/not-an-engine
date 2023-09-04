#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "core/Camera.hpp"
#include "core/Entity.hpp"
#include "core/GraphicContext.hpp"
#include "renderer/Buffer.hpp"
#include "renderer/Descriptor.hpp"
#include "renderer/Pipeline.hpp"
#include "renderer/Texture.hpp"

namespace nae {

class Renderer {
public:
    Renderer();
    ~Renderer() = default;

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;
    Renderer(Renderer &&) = default;
    Renderer &operator=(Renderer &&) = default;

    bool beginFrame();
    void beginScene(const Camera &camera);
    void endScene();
    bool endFrame();

    void draw(const Entity &entity, const Texture &texture);

    [[nodiscard]] Pipeline &getPipeline() const noexcept { return *pPipeline_; };
    [[nodiscard]] const DescriptorPool &getDescriptorPool() const noexcept { return *pDescriptorPool_; };
    [[nodiscard]] const vk::raii::CommandPool &getVkCommandPool() const noexcept { return *pVkCommandPool_; };
    [[nodiscard]] const vk::raii::Sampler &getVkSampler() const noexcept { return *pVkSampler_; };

private:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    std::unique_ptr<DescriptorPool> pDescriptorPool_;
    std::unique_ptr<Pipeline> pPipeline_;
    std::unique_ptr<vk::raii::CommandPool> pVkCommandPool_;
    std::unique_ptr<vk::raii::CommandBuffers> pVkCommandBuffers_;
    std::vector<Buffer> mvpBuffers_;
    std::unique_ptr<DescriptorSets> pDescriptorSets_;
    std::unique_ptr<vk::raii::Sampler> pVkSampler_;

    std::vector<vk::raii::Semaphore> imageAcquiredSemaphores_;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores_;
    std::vector<vk::raii::Fence> drawFences_;

    std::vector<vk::DescriptorSet> descriptorSetsToBind_;

    uint32_t currentFrameBufferIdx_{0};
    uint32_t currentCommandBufferIdx_{0};
};

} // namespace nae
