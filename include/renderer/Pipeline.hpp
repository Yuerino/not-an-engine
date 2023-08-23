#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Descriptor.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"
#include "core/Device.hpp"
#include "core/Swapchain.hpp"

namespace nae {

class Pipeline {
public:
    Pipeline(const Device &device,
             const Swapchain &swapChain,
             const DescriptorSetLayout &descriptorSetLayout,
             const std::string &vertexShaderPath,
             const std::string &fragmentShaderPath);
    ~Pipeline() = default;

    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;
    Pipeline(Pipeline &&) = default;
    Pipeline &operator=(Pipeline &&) = default;

    [[nodiscard]] const vk::raii::Pipeline &get() const noexcept;
    [[nodiscard]] const RenderPass &getRenderPass() const noexcept;
    [[nodiscard]] const vk::raii::PipelineLayout &getLayout() const noexcept;

private:
    ShaderModule vertexShaderModule_{nullptr};
    ShaderModule fragmentShaderModule_{nullptr};

    RenderPass renderPass_{nullptr};

    vk::raii::PipelineLayout vkPipelineLayout_{nullptr};
    vk::raii::PipelineCache vkPipelineCache_{nullptr};
    vk::raii::Pipeline vkPipeline_{nullptr};
};

} // namespace nae
