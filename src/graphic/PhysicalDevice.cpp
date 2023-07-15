#include "graphic/PhysicalDevice.hpp"

namespace nae::graphic {

vk::raii::PhysicalDevice createPhysicalDevice(const vk::raii::Instance &instance) {
    // TODO: refactor to handle empty physical devices and select the better physical devices
    return vk::raii::PhysicalDevices(instance).front();
}

} // namespace nae::graphic
