#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "Window.hpp"
#include "graphic/Buffer.hpp"
#include "graphic/Device.hpp"
#include "graphic/Image.hpp"
#include "graphic/Instance.hpp"
#include "graphic/PhysicalDevice.hpp"
#include "graphic/Surface.hpp"
#include "graphic/SwapChain.hpp"

namespace nae {

class Graphic {
public:
    explicit Graphic(const Window &window);
    ~Graphic();

    Graphic(const Graphic &) = delete;
    Graphic &operator=(const Graphic &) = delete;

    void Update();

private:
    const Window &window_;

    graphic::Instance instance_;
    graphic::Surface surface_;
    graphic::PhysicalDevice physicalDevice_;
    graphic::Device device_;
    vk::raii::CommandPool vkCommandPool_{nullptr};
    vk::raii::CommandBuffer vkCommandBuffer_{nullptr};
    std::unique_ptr<graphic::SwapChainData> SwapChainDataPtr_;
    graphic::DepthBufferData depthBufferData_{nullptr};
    graphic::BufferData uniformBufferData_{nullptr};
    glm::mat4x4 modelMatrix_{};
    vk::raii::DescriptorSetLayout vkDescriptorSetLayout_{nullptr};
    vk::raii::PipelineLayout vkPipelineLayout_{nullptr};
    vk::raii::RenderPass vkRenderPass_{nullptr};
    std::vector<vk::raii::Framebuffer> frameBuffers_{};
    graphic::BufferData vertexBufferData_{nullptr};
    vk::raii::DescriptorPool vkDescriptorPool_{nullptr};
    vk::raii::DescriptorSet vkDescriptorSet_{nullptr};
    vk::raii::ShaderModule vkVertexShaderModule_{nullptr};
    vk::raii::ShaderModule vkFragmentShaderModule_{nullptr};
    vk::raii::PipelineCache vkPipelineCache_{nullptr};
    vk::raii::Pipeline graphicPipeline_{nullptr};
};

} // namespace nae
