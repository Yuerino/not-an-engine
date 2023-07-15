#include "graphic/Device.hpp"

#include <cassert>

namespace nae::graphic {

namespace {

    uint32_t findQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties> &queueFamilyProperties,
                                  vk::QueueFlagBits queueFlagBits) {
        auto queueFamilyPropertyIt =
                std::find_if(queueFamilyProperties.begin(),
                             queueFamilyProperties.end(),
                             [&queueFlagBits](const vk::QueueFamilyProperties &queueFamilyProperty) {
                                 return queueFamilyProperty.queueFlags & queueFlagBits;
                             });
        assert(queueFamilyPropertyIt != queueFamilyProperties.end());
        return static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), queueFamilyPropertyIt));
    }

} // namespace

vk::raii::Device createDevice(const vk::raii::PhysicalDevice &physicalDevice) {
    // TODO: refactor to pass enabled layers, extensions and features to device
    float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfos{
            {},
            findQueueFamilyIndex(physicalDevice.getQueueFamilyProperties(), vk::QueueFlagBits::eGraphics),
            1,
            &queuePriority};
    vk::DeviceCreateInfo deviceCreateInfo{{}, deviceQueueCreateInfos};

    return vk::raii::Device{physicalDevice, deviceCreateInfo};
}

} // namespace nae::graphic
