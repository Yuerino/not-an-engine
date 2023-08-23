#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "core/GraphicInstance.hpp"
#include "renderer/Buffer.hpp"
#include "renderer/Descriptor.hpp"
#include "renderer/Pipeline.hpp"

namespace nae {

class Renderer {
public:
    explicit Renderer(const GraphicInstance &graphicInstance);
    ~Renderer() = default;

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;
    Renderer(Renderer &&) = default;
    Renderer &operator=(Renderer &&) = default;

    bool beginFrame();
    void beginScene();
    void endScene();
    bool endFrame();

    Buffer loadVertices(const std::vector<Vertex> &vertices);
    void draw(const Buffer &buffer, uint32_t vertexCount);

    [[nodiscard]] const Pipeline &getPipeline() const noexcept;

private:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    const GraphicInstance *pGraphicInstance_;
    DescriptorPool descriptorPool_;
    DescriptorSetLayout descriptorSetLayout_;
    Pipeline pipeline_;
    vk::raii::CommandPool vkCommandPool_{nullptr};
    vk::raii::CommandBuffers vkCommandBuffers_{nullptr};
    std::vector<Buffer> mvpBuffers_;
    std::unique_ptr<DescriptorSets> pDescriptorSets_;

    std::vector<vk::raii::Semaphore> imageAcquiredSemaphores_;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores_;
    std::vector<vk::raii::Fence> drawFences_;

    uint32_t currentFrameBufferIdx_{0};
    uint32_t currentCommandBufferIdx_{0};
};

} // namespace nae
