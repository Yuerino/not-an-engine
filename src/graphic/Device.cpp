#include "graphic/Device.hpp"

#include <cassert>

namespace nae::graphic {

[[nodiscard]] static uint32_t findQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties> &queueFamilyProperties,
                                                   vk::QueueFlags queueFlags);

[[nodiscard]] static uint32_t findMemoryType(const vk::PhysicalDeviceMemoryProperties &memoryProperties,
                                             uint32_t typeBits,
                                             vk::MemoryPropertyFlags requirementsMask);

Device::Device(const PhysicalDevice &physicalDevice, const Surface &surface) : physicalDevice_{physicalDevice} {
    setGraphicAndPresentQueueFamilyIndex(surface);

    auto extensions = physicalDevice_.getExtensions();
    std::vector<const char *> enabledExtensions;
    enabledExtensions.reserve(extensions.size());
    for (const auto &extension: extensions) {
        enabledExtensions.push_back(extension.c_str());
    }

    float queuePriority = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfos{vk::DeviceQueueCreateFlags{},
                                                     graphicQueueFamilyIndex_,
                                                     1,
                                                     &queuePriority};
    // TODO: pass physical device features
    vk::DeviceCreateInfo deviceCreateInfo{vk::DeviceCreateFlags{}, deviceQueueCreateInfos, {}, enabledExtensions};

    vkDevice_ = vk::raii::Device{physicalDevice_.get(), deviceCreateInfo};

    vkGraphicQueue_ = vk::raii::Queue{vkDevice_, graphicQueueFamilyIndex_, 0};
    vkPresentQueue_ = vk::raii::Queue{vkDevice_, presentQueueFamilyIndex_, 0};
}

vk::raii::DeviceMemory Device::createDeviceMemory(const vk::MemoryRequirements &memoryRequirements,
                                                  vk::MemoryPropertyFlags memoryPropertyFlags) const {
    uint32_t memoryTypeIndex = findMemoryType(physicalDevice_.get().getMemoryProperties(),
                                              memoryRequirements.memoryTypeBits,
                                              memoryPropertyFlags);

    return {vkDevice_, vk::MemoryAllocateInfo{memoryRequirements.size, memoryTypeIndex}};
}

const vk::raii::Queue &Device::getGraphicQueue() const noexcept {
    return vkGraphicQueue_;
}

const vk::raii::Queue &Device::getPresentQueue() const noexcept {
    return vkPresentQueue_;
}

uint32_t Device::getGraphicQueueFamilyIndex() const noexcept {
    return graphicQueueFamilyIndex_;
}

uint32_t Device::getPresentQueueFamilyIndex() const noexcept {
    return presentQueueFamilyIndex_;
}

const vk::raii::Device &Device::get() const noexcept {
    return vkDevice_;
}

void Device::setGraphicAndPresentQueueFamilyIndex(const Surface &surface) {
    auto queueFamilyProperties = physicalDevice_.get().getQueueFamilyProperties();
    auto graphicQueueFamilyIndex = findQueueFamilyIndex(queueFamilyProperties, vk::QueueFlagBits::eGraphics);

    // If graphic queue family index supports present then returns it
    if (physicalDevice_.get().getSurfaceSupportKHR(graphicQueueFamilyIndex, *surface.get()) &&
        queueFamilyProperties[graphicQueueFamilyIndex].queueCount > 1) {
        graphicQueueFamilyIndex_ = graphicQueueFamilyIndex;
        presentQueueFamilyIndex_ = graphicQueueFamilyIndex;
    }

    // Looks for family index that supports both graphic and presents
    for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
        if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
            physicalDevice_.get().getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface.get()) &&
            queueFamilyProperties[i].queueCount > 1) {
            graphicQueueFamilyIndex_ = static_cast<uint32_t>(i);
            presentQueueFamilyIndex_ = static_cast<uint32_t>(i);
        }
    }

    // If there's no family index that supports both graphic and presents then looks for family index that supports
    // present
    for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
        if (physicalDevice_.get().getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface.get())) {
            graphicQueueFamilyIndex_ = graphicQueueFamilyIndex;
            presentQueueFamilyIndex_ = static_cast<uint32_t>(i);
        }
    }
}

static uint32_t findQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties> &queueFamilyProperties,
                                     vk::QueueFlags queueFlags) {
    auto queueFamilyPropertyIt = std::find_if(queueFamilyProperties.begin(),
                                              queueFamilyProperties.end(),
                                              [&queueFlags](const vk::QueueFamilyProperties &queueFamilyProperty) {
                                                  return queueFamilyProperty.queueFlags & queueFlags;
                                              });
    assert(queueFamilyPropertyIt != queueFamilyProperties.end());
    return static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), queueFamilyPropertyIt));
}

static uint32_t findMemoryType(const vk::PhysicalDeviceMemoryProperties &memoryProperties,
                               uint32_t typeBits,
                               vk::MemoryPropertyFlags requirementsMask) {
    auto typeIndex = uint32_t(~0);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((typeBits & 1) &&
            ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)) {
            typeIndex = i;
            break;
        }
        typeBits >>= 1;
    }
    assert(typeIndex != uint32_t(~0));
    return typeIndex;
}

} // namespace nae::graphic
