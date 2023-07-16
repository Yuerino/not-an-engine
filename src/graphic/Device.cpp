#include "graphic/Device.hpp"

#include <cassert>

namespace nae::graphic {

namespace {

    [[nodiscard]] uint32_t findQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties> &queueFamilyProperties,
                                                vk::QueueFlagBits queueFlagBits) {
        auto queueFamilyPropertyIt = std::find_if(
                queueFamilyProperties.begin(),
                queueFamilyProperties.end(),
                [&queueFlagBits](const vk::QueueFamilyProperties &queueFamilyProperty) {
                    return queueFamilyProperty.queueFlags & queueFlagBits;
                });
        assert(queueFamilyPropertyIt != queueFamilyProperties.end());
        return static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), queueFamilyPropertyIt));
    }

} // namespace

// TODO: refactor to pass enabled layers and features to device
vk::raii::Device createDevice(const vk::raii::PhysicalDevice &physicalDevice,
                              uint32_t graphicQueueFamilyIndex,
                              const std::vector<std::string> &extensions) {
    std::vector<const char *> enabledExtensions;
    enabledExtensions.reserve(extensions.size());
    for (const auto &extension: extensions) {
        enabledExtensions.push_back(extension.c_str());
    }

    float queuePriority = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfos{vk::DeviceQueueCreateFlags{},
                                                     graphicQueueFamilyIndex,
                                                     1,
                                                     &queuePriority};
    vk::DeviceCreateInfo deviceCreateInfo{vk::DeviceCreateFlags{}, deviceQueueCreateInfos, {}, enabledExtensions};

    return vk::raii::Device{physicalDevice, deviceCreateInfo};
}

std::pair<uint32_t, uint32_t> findGraphicAndPresentQueueFamilyIndex(const vk::raii::PhysicalDevice &physicalDevice,
                                                                    const vk::raii::SurfaceKHR &surface) {
    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto graphicQueueFamilyIndex = findQueueFamilyIndex(queueFamilyProperties, vk::QueueFlagBits::eGraphics);

    // If graphic queue family index supports present then returns it
    if (physicalDevice.getSurfaceSupportKHR(graphicQueueFamilyIndex, *surface)) {
        return {graphicQueueFamilyIndex, graphicQueueFamilyIndex};
    }

    // Looks for family index that supports both graphic and presents
    for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
        if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
            physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface)) {
            return {static_cast<uint32_t>(i), static_cast<uint32_t>(i)};
        }
    }

    // If there's no family index that supports both graphic and presents then looks for family index that supports
    // present
    for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
        if (physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface)) {
            return {graphicQueueFamilyIndex, static_cast<uint32_t>(i)};
        }
    }

    throw std::runtime_error("Failed to find graphic and present queue family index");
}

} // namespace nae::graphic
