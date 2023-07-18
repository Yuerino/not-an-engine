#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "graphic/PhysicalDevice.hpp"
#include "graphic/Surface.hpp"

namespace nae::graphic {

class PhysicalDevice;
class Surface;

class Device {
public:
    explicit Device(const PhysicalDevice &physicalDevice, const Surface &surface);

    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;

    [[nodiscard]] vk::raii::DeviceMemory createDeviceMemory(const vk::MemoryRequirements &memoryRequirements,
                                                            vk::MemoryPropertyFlags memoryPropertyFlags) const;

    [[nodiscard]] const vk::raii::Queue &getGraphicQueue() const noexcept;
    [[nodiscard]] const vk::raii::Queue &getPresentQueue() const noexcept;
    [[nodiscard]] uint32_t getGraphicQueueFamilyIndex() const noexcept;
    [[nodiscard]] uint32_t getPresentQueueFamilyIndex() const noexcept;
    [[nodiscard]] const vk::raii::Device &get() const noexcept;

private:
    void setGraphicAndPresentQueueFamilyIndex(const Surface &surface);

    const PhysicalDevice &physicalDevice_;

    vk::raii::Device vkDevice_{nullptr};
    vk::raii::Queue vkGraphicQueue_{nullptr};
    vk::raii::Queue vkPresentQueue_{nullptr};

    uint32_t graphicQueueFamilyIndex_{};
    uint32_t presentQueueFamilyIndex_{};
};

template<typename T>
void copyToDevice(const vk::raii::DeviceMemory &deviceMemory,
                  const T *pData,
                  size_t count,
                  vk::DeviceSize stride = sizeof(T)) {
    assert(sizeof(T) <= stride);
    auto *deviceData = static_cast<uint8_t *>(deviceMemory.mapMemory(0, count * stride));

    if (stride == sizeof(T)) {
        memcpy(deviceData, pData, count * sizeof(T));
    } else {
        for (size_t i = 0; i < count; ++i) {
            memcpy(deviceData, &pData[i], sizeof(T));
            deviceData += stride;
        }
    }

    deviceMemory.unmapMemory();
}

template<typename T>
void copyToDevice(const vk::raii::DeviceMemory &deviceMemory, const T &data) {
    copyToDevice<T>(deviceMemory, &data, 1);
}

} // namespace nae::graphic
