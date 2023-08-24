#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "core/GraphicContext.hpp"

namespace nae {

class GraphicContext;

class PhysicalDevice {
public:
    PhysicalDevice(const GraphicContext &graphicContext, const std::vector<std::string> &extensions = {}) noexcept;
    ~PhysicalDevice() = default;

    PhysicalDevice(const PhysicalDevice &) = delete;
    PhysicalDevice &operator=(const PhysicalDevice &) = delete;
    PhysicalDevice(PhysicalDevice &&) = default;
    PhysicalDevice &operator=(PhysicalDevice &&) = default;

    [[nodiscard]] const std::vector<std::string> &getExtensions() const noexcept;
    [[nodiscard]] const vk::raii::PhysicalDevice &get() const noexcept;

private:
    [[nodiscard]] vk::raii::PhysicalDevice choosePhysicalDevice(const vk::raii::PhysicalDevices &physicalDevices,
                                                                const vk::raii::SurfaceKHR &vkSurface) const;

    [[nodiscard]] uint32_t scorePhysicalDevice(const vk::raii::PhysicalDevice &physicalDevice,
                                               const vk::raii::SurfaceKHR &vkSurface) const;

    vk::raii::PhysicalDevice vkPhysicalDevice_{nullptr};
    std::vector<std::string> extensions_;
};

} // namespace nae
