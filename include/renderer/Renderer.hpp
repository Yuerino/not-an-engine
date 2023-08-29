#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "core/Camera.hpp"
#include "core/GraphicContext.hpp"
#include "renderer/Buffer.hpp"
#include "renderer/Descriptor.hpp"
#include "renderer/Pipeline.hpp"

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

    Buffer loadVertices(const std::vector<Vertex> &vertices);
    void draw(const Buffer &buffer, uint32_t vertexCount);

    [[nodiscard]] Pipeline &getPipeline() const noexcept { return *pPipeline_; };

private:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    std::unique_ptr<DescriptorPool> pDescriptorPool_;
    std::unique_ptr<DescriptorSetLayout> pDescriptorSetLayout_;
    std::unique_ptr<Pipeline> pPipeline_;
    std::unique_ptr<vk::raii::CommandPool> pVkCommandPool_;
    std::unique_ptr<vk::raii::CommandBuffers> pVkCommandBuffers_;
    std::vector<Buffer> mvpBuffers_;
    std::unique_ptr<DescriptorSets> pDescriptorSets_;

    std::vector<vk::raii::Semaphore> imageAcquiredSemaphores_;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores_;
    std::vector<vk::raii::Fence> drawFences_;

    uint32_t currentFrameBufferIdx_{0};
    uint32_t currentCommandBufferIdx_{0};
};

} // namespace nae
