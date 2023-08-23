#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "core/PhysicalDevice.hpp"

namespace nae {

class PhysicalDevice;

class Surface {
public:
    explicit Surface(vk::raii::SurfaceKHR &&vkSurface);
    ~Surface() = default;

    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;
    Surface(Surface &&) = default;
    Surface &operator=(Surface &&) = default;

    vk::SurfaceFormatKHR pickSurfaceFormat(const PhysicalDevice &physicalDevice) const;

    vk::PresentModeKHR pickPresentMode(const PhysicalDevice &physicalDevice) const;

    [[nodiscard]] const vk::raii::SurfaceKHR &get() const noexcept;

private:
    vk::raii::SurfaceKHR vkSurface_{nullptr};
};

} // namespace nae
