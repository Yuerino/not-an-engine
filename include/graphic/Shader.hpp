#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "graphic/Device.hpp"

namespace nae::graphic {

class ShaderModule {
public:
    ShaderModule(const Device &device, const std::string &shaderPath);
    explicit ShaderModule(std::nullptr_t) {}

    ShaderModule(const ShaderModule &) = delete;
    ShaderModule &operator=(const ShaderModule &) = delete;
    ShaderModule(ShaderModule &&) noexcept = default;
    ShaderModule &operator=(ShaderModule &&) noexcept = default;

    [[nodiscard]] const vk::raii::ShaderModule &get() const noexcept;

private:
    vk::raii::ShaderModule vkShaderModule_{nullptr};
};

} // namespace nae::graphic
