#pragma once

#include <functional>
#include <string>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "core/PhysicalDevice.hpp"
#include "core/Surface.hpp"

namespace nae {

class PhysicalDevice;
class Surface;

class Device {
public:
    explicit Device(const PhysicalDevice &physicalDevice, const Surface &surface);
    ~Device() = default;

    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;
    Device(Device &&) = default;
    Device &operator=(Device &&) = default;

    [[nodiscard]] vk::raii::DeviceMemory createDeviceMemory(const vk::MemoryRequirements &memoryRequirements,
                                                            vk::MemoryPropertyFlags memoryPropertyFlags) const;

    [[nodiscard]] const vk::raii::Queue &getGraphicQueue() const noexcept;
    [[nodiscard]] const vk::raii::Queue &getPresentQueue() const noexcept;
    [[nodiscard]] uint32_t getGraphicQueueFamilyIndex() const noexcept;
    [[nodiscard]] uint32_t getPresentQueueFamilyIndex() const noexcept;
    [[nodiscard]] const vk::raii::Device &get() const noexcept;

private:
    void setGraphicAndPresentQueueFamilyIndex(const vk::raii::SurfaceKHR &vkSurface);

    const PhysicalDevice *pPhysicalDevice_;

    vk::raii::Device vkDevice_{nullptr};
    vk::raii::Queue vkGraphicQueue_{nullptr};
    vk::raii::Queue vkPresentQueue_{nullptr};

    uint32_t graphicQueueFamilyIndex_{};
    uint32_t presentQueueFamilyIndex_{};
};

} // namespace nae
