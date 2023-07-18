#include "graphic/PhysicalDevice.hpp"

#include <map>

namespace nae::graphic {

[[nodiscard]] static bool physicalDeviceSupportQueueFamily(const vk::raii::PhysicalDevice &physicalDevice,
                                                           const Surface &surface,
                                                           vk::QueueFlags queueFlags);

PhysicalDevice::PhysicalDevice(const Instance &instance,
                               const Surface &surface,
                               const std::vector<std::string> &extensions)
    : extensions_{extensions} {
    vk::raii::PhysicalDevices physicalDevices(instance.get());
    assert(!physicalDevices.empty());

    vkPhysicalDevice_ = choosePhysicalDevice(physicalDevices, surface);
    if (!*vkPhysicalDevice_) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}

const std::vector<std::string> &PhysicalDevice::getExtensions() const noexcept {
    return extensions_;
}

const vk::raii::PhysicalDevice &PhysicalDevice::get() const noexcept {
    return vkPhysicalDevice_;
}

vk::raii::PhysicalDevice PhysicalDevice::choosePhysicalDevice(const vk::raii::PhysicalDevices &physicalDevices,
                                                              const Surface &surface) const {
    std::multimap<uint32_t, vk::raii::PhysicalDevice> rankedPhysicalDevices;

    for (const auto &physicalDevice: physicalDevices) {
        rankedPhysicalDevices.emplace(scorePhysicalDevice(physicalDevice, surface), physicalDevice);
    }

    if (!rankedPhysicalDevices.empty() && rankedPhysicalDevices.rbegin()->first > 0) {
        return rankedPhysicalDevices.rbegin()->second;
    }

    return {nullptr};
}

uint32_t PhysicalDevice::scorePhysicalDevice(const vk::raii::PhysicalDevice &physicalDevice,
                                             const Surface &surface) const {
    // TODO: check for device features
    // TODO: log device properties and score
    uint32_t score = 0;
    auto extensionProperties = physicalDevice.enumerateDeviceExtensionProperties();

    for (const auto &extension: extensions_) {
        if (std::none_of(extensionProperties.begin(),
                         extensionProperties.end(),
                         [&extension](const auto &extensionProperty) {
                             return extensionProperty.extensionName == extension;
                         })) {
            return 0;
        }
    }

    if (!physicalDeviceSupportQueueFamily(physicalDevice, surface, vk::QueueFlagBits::eGraphics)) {
        return 0;
    }

    if (physicalDevice.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    // max 2D texture size
    score += physicalDevice.getProperties().limits.maxImageDimension2D;

    return score;
}

static bool physicalDeviceSupportQueueFamily(const vk::raii::PhysicalDevice &physicalDevice,
                                             const Surface &surface,
                                             vk::QueueFlags queueFlags) {
    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    bool supportPresentQueue = false;

    for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
        // If flag empty then no need to check
        if (!queueFlags) {
            break;
        }

        // Check if the queue family support one of the queue flags, if yes then remove it from flags
        if (queueFamilyProperties[i].queueFlags & queueFlags) {
            queueFlags &= ~queueFamilyProperties[i].queueFlags;
        }

        // If there's surface then check if the queue family support present for this surface
        if (!!*surface.get() && physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface.get())) {
            supportPresentQueue = true;
        }
    }

    if (!*surface.get()) {
        return !queueFlags;
    }
    return !queueFlags && supportPresentQueue;
}

} // namespace nae::graphic
