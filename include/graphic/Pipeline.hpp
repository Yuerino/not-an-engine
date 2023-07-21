#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "graphic/Device.hpp"
#include "graphic/RenderPass.hpp"
#include "graphic/Shader.hpp"
#include "graphic/SwapChain.hpp"

namespace nae::graphic {

class Pipeline {
public:
    Pipeline(const Device &device,
             std::shared_ptr<SwapChain> pSwapChain,
             const std::string &vertexShaderPath,
             const std::string &fragmentShaderPath);

    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;

    [[nodiscard]] const vk::raii::Pipeline &get() const noexcept;
    [[nodiscard]] const RenderPass &getRenderPass() const noexcept;

private:
    std::shared_ptr<SwapChain> pSwapChain_;

    ShaderModule vertexShaderModule_{nullptr};
    ShaderModule fragmentShaderModule_{nullptr};

    RenderPass renderPass_{nullptr};

    vk::raii::PipelineLayout vkPipelineLayout_{nullptr};
    vk::raii::PipelineCache vkPipelineCache_{nullptr};
    vk::raii::Pipeline vkPipeline_{nullptr};
};

} // namespace nae::graphic
