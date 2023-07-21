#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "Window.hpp"
#include "graphic/Instance.hpp"
#include "graphic/PhysicalDevice.hpp"

namespace nae::graphic {

class PhysicalDevice;

class Surface {
public:
    Surface(const Instance &instance, const ::nae::Window &window);

    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;

    [[nodiscard]] vk::SurfaceFormatKHR pickSurfaceFormat(const PhysicalDevice &physicalDevice) const;

    [[nodiscard]] vk::PresentModeKHR pickPresentMode(const PhysicalDevice &physicalDevice) const;

    [[nodiscard]] const vk::raii::SurfaceKHR &get() const noexcept;
    [[nodiscard]] const ::nae::Window &getWindow() const noexcept;

private:
    const Window &window_;
    vk::raii::SurfaceKHR vkSurface_{nullptr};
};

} // namespace nae::graphic
