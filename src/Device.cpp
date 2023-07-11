#include "Device.hpp"

#include "util/vulkan.hpp"

namespace nae {

nae::Device::Device()
    : vkContext_{},
      vkInstance_{util::vk::createInstance(vkContext_,
                                           "Vulkan window",
                                           VK_MAKE_API_VERSION(0, 1, 0, 0),
                                           "Not an engine",
                                           VK_MAKE_API_VERSION(0, 1, 0, 0))},
      vkPhysicalDevice_{vk::raii::PhysicalDevices(vkInstance_).front()},
      vkDevice_{util::vk::createDevice(vkPhysicalDevice_)}
#ifndef NDEBUG
      ,
      vkDebugUtilsMessenger_{vkInstance_, util::vk::makeDebugUtilsMessengerCreateInfoEXT()}
#endif
{
    // TODO: refactor to handle empty physical devices and select the better physical devices
    // TODO: refactor to pass enabled layers, extensions and features to device
}

} // namespace nae
