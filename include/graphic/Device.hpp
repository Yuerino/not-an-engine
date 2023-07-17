#ifndef NOT_AN_ENGINE_GRAPHIC_DEVICE_HPP
#define NOT_AN_ENGINE_GRAPHIC_DEVICE_HPP

#include <string>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

[[nodiscard]] vk::raii::Device createDevice(const vk::raii::PhysicalDevice &physicalDevice,
                                            uint32_t graphicQueueFamilyIndex,
                                            const std::vector<std::string> &extensions = {});

[[nodiscard]] std::pair<uint32_t, uint32_t>
findGraphicAndPresentQueueFamilyIndex(const vk::raii::PhysicalDevice &physicalDevice,
                                      const vk::raii::SurfaceKHR &surface);

[[nodiscard]] vk::raii::DeviceMemory createDeviceMemory(const vk::raii::Device &device,
                                                        const vk::PhysicalDeviceMemoryProperties &memoryProperties,
                                                        const vk::MemoryRequirements &memoryRequirements,
                                                        vk::MemoryPropertyFlags memoryPropertyFlags);

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

#endif // NOT_AN_ENGINE_GRAPHIC_DEVICE_HPP
