#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Shader.hpp"

namespace nae {

class Pipeline {
public:
    Pipeline(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    [[nodiscard]] const vk::raii::Pipeline &get() const noexcept { return vkPipeline_; }
    [[nodiscard]] const vk::raii::PipelineLayout &getLayout() const noexcept { return vkPipelineLayout_; }

private:
    ShaderModule vertexShaderModule_{nullptr};
    ShaderModule fragmentShaderModule_{nullptr};

    vk::raii::PipelineLayout vkPipelineLayout_{nullptr};
    vk::raii::PipelineCache vkPipelineCache_{nullptr};
    vk::raii::Pipeline vkPipeline_{nullptr};
};

} // namespace nae
