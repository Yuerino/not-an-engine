#include "core/PhysicalDevice.hpp"

#include <map>

namespace nae {

[[nodiscard]] static bool physicalDeviceSupportQueueFamily(const vk::raii::PhysicalDevice &physicalDevice,
                                                           const vk::raii::SurfaceKHR &vkSurface,
                                                           vk::QueueFlags queueFlags);

PhysicalDevice::PhysicalDevice(const GraphicContext &graphicContext,
                               const std::vector<std::string> &extensions) noexcept
    : extensions_{extensions} {
    vk::raii::PhysicalDevices physicalDevices(graphicContext.get());
    assert(not physicalDevices.empty() && "Failed to find GPUs (wut)");

    vkPhysicalDevice_ = choosePhysicalDevice(physicalDevices, graphicContext.getSurface().get());
    assert(*vkPhysicalDevice_ && "Failed to find a suitable GPU!");
}

const std::vector<std::string> &PhysicalDevice::getExtensions() const noexcept {
    return extensions_;
}

const vk::raii::PhysicalDevice &PhysicalDevice::get() const noexcept {
    return vkPhysicalDevice_;
}

vk::raii::PhysicalDevice PhysicalDevice::choosePhysicalDevice(const vk::raii::PhysicalDevices &physicalDevices,
                                                              const vk::raii::SurfaceKHR &vkSurface) const {
    std::multimap<uint32_t, vk::raii::PhysicalDevice> rankedPhysicalDevices;

    for (const auto &physicalDevice: physicalDevices) {
        rankedPhysicalDevices.emplace(scorePhysicalDevice(physicalDevice, vkSurface), physicalDevice);
    }

    if (!rankedPhysicalDevices.empty() && rankedPhysicalDevices.rbegin()->first > 0) {
        return rankedPhysicalDevices.rbegin()->second;
    }

    return {nullptr};
}

uint32_t PhysicalDevice::scorePhysicalDevice(const vk::raii::PhysicalDevice &physicalDevice,
                                             const vk::raii::SurfaceKHR &vkSurface) const {
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

    if (!physicalDeviceSupportQueueFamily(physicalDevice, vkSurface, vk::QueueFlagBits::eGraphics)) {
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
                                             const vk::raii::SurfaceKHR &vkSurface,
                                             vk::QueueFlags queueFlags) {
    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    // If there's surface then check if the queue family support present for this surface
    bool supportSurfacePresentQueue = !*vkSurface;

    for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
        // Check if the queue family support one of the queue flags, if yes then remove it from flags
        if (queueFamilyProperties[i].queueFlags & queueFlags) {
            queueFlags &= ~queueFamilyProperties[i].queueFlags;
        }

        if (!supportSurfacePresentQueue && physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *vkSurface)) {
            supportSurfacePresentQueue = true;
        }
    }

    return queueFlags == vk::QueueFlags{} && supportSurfacePresentQueue;
}

} // namespace nae
