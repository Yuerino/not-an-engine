#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "graphic/Device.hpp"
#include "graphic/Instance.hpp"
#include "graphic/Surface.hpp"

namespace nae::graphic {

class PhysicalDevice {
public:
    PhysicalDevice(const Instance &instance, const Surface &surface, const std::vector<std::string> &extensions = {});

    PhysicalDevice(const PhysicalDevice &) = delete;
    PhysicalDevice &operator=(const PhysicalDevice &) = delete;

    [[nodiscard]] const std::vector<std::string> &getExtensions() const noexcept;
    [[nodiscard]] const vk::raii::PhysicalDevice &get() const noexcept;

private:
    [[nodiscard]] vk::raii::PhysicalDevice choosePhysicalDevice(const vk::raii::PhysicalDevices &physicalDevices,
                                                                const Surface &surface) const;

    [[nodiscard]] uint32_t scorePhysicalDevice(const vk::raii::PhysicalDevice &physicalDevice,
                                               const Surface &surface) const;

    vk::raii::PhysicalDevice vkPhysicalDevice_{nullptr};
    std::vector<std::string> extensions_;
};

} // namespace nae::graphic
