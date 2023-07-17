#ifndef NOT_AN_ENGINE_GRAPHIC_SHADER_HPP
#define NOT_AN_ENGINE_GRAPHIC_SHADER_HPP

#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

[[nodiscard]] vk::raii::ShaderModule
createShaderModule(const vk::raii::Device &device, vk::ShaderStageFlagBits shaderStage, const std::string &shaderPath);

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_SHADER_HPP
