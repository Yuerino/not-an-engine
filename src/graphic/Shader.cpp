#include "graphic/Shader.hpp"

#include "util.hpp"

namespace nae::graphic {

ShaderModule::ShaderModule(const Device &device, const std::string &shaderPath) {
    auto shaderCode = util::readFile(shaderPath);
    vkShaderModule_ = vk::raii::ShaderModule{
            device.get(),
            {vk::ShaderModuleCreateFlags{}, shaderCode.size(), reinterpret_cast<const uint32_t *>(shaderCode.data())}};
}

const vk::raii::ShaderModule &ShaderModule::get() const noexcept {
    return vkShaderModule_;
}

} // namespace nae::graphic
