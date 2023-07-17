#ifndef NOT_AN_ENGINE_GRAPHIC_GRAPHIC_HPP
#define NOT_AN_ENGINE_GRAPHIC_GRAPHIC_HPP

#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "Window.hpp"
#include "graphic/Buffer.hpp"
#include "graphic/Image.hpp"
#include "graphic/SwapChain.hpp"

namespace nae {

class Graphic {
public:
    explicit Graphic(const Window &window);
    ~Graphic() = default;

    Graphic(const Graphic &) = delete;
    Graphic &operator=(const Graphic &) = delete;
    Graphic(Graphic &&) = delete;
    Graphic &operator=(Graphic &&) = delete;

    void Update();

private:
    const Window &window_;
    vk::raii::Context vkContext_;
    vk::raii::Instance vkInstance_;
    std::unique_ptr<vk::raii::PhysicalDevice> vkPhysicalDevicePtr_;
    std::unique_ptr<vk::raii::SurfaceKHR> vkSurfacePtr_;
    std::unique_ptr<vk::raii::Device> vkDevicePtr_;
    vk::raii::CommandPool vkCommandPool_{nullptr};
    vk::raii::CommandBuffer vkCommandBuffer_{nullptr};
    vk::raii::Queue vkGraphicQueue_{nullptr};
    vk::raii::Queue vkPresentQueue_{nullptr};
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

    uint32_t graphicQueueFamilyIndex_;
    uint32_t presentQueueFamilyIndex_;

#if !defined(NDEBUG)
    vk::raii::DebugUtilsMessengerEXT vkDebugUtilsMessenger_;
#endif
};

} // namespace nae

#endif // NOT_AN_ENGINE_GRAPHIC_GRAPHIC_HPP
