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
                             return extension == extensionProperty.extensionName;
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

/*!
 * @brief Check if the physical device support queue families
 * @param surface Surface to check if physical device has present queue family for this surface
 * @param queueFlags The queue family flags to check
 */
static bool physicalDeviceSupportQueueFamily(const vk::raii::PhysicalDevice &physicalDevice,
                                             const Surface &surface,
                                             vk::QueueFlags queueFlags) {
    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    // If there's surface then check if the queue family support present for this surface
    bool supportSurfacePresentQueue = !*surface.get();

    for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
        // Check if the queue family support one of the queue flags, if yes then remove it from flags
        if (queueFamilyProperties[i].queueFlags & queueFlags) {
            queueFlags &= ~queueFamilyProperties[i].queueFlags;
        }

        if (!supportSurfacePresentQueue &&
            physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface.get())) {
            supportSurfacePresentQueue = true;
        }
    }

    return queueFlags == vk::QueueFlags{} && supportSurfacePresentQueue;
}

} // namespace nae::graphic
