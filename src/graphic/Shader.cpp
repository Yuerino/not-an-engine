#include "graphic/Shader.hpp"

#include "util.hpp"

namespace nae::graphic {

vk::raii::ShaderModule
createShaderModule(const vk::raii::Device &device, vk::ShaderStageFlagBits shaderStage, const std::string &shaderPath) {
    auto shaderCode = util::readFile(shaderPath);
    return {device,
            {vk::ShaderModuleCreateFlags{}, shaderCode.size(), reinterpret_cast<const uint32_t *>(shaderCode.data())}};
}

} // namespace nae::graphic
