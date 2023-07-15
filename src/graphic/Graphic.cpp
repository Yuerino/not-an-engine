#include "graphic/Graphic.hpp"

#include "graphic/Device.hpp"
#include "graphic/Instance.hpp"
#include "graphic/PhysicalDevice.hpp"

namespace nae {

nae::Graphic::Graphic()
    : vkContext_{},
      vkInstance_{graphic::createInstance(vkContext_,
                                          "Vulkan window",
                                          VK_MAKE_API_VERSION(0, 1, 0, 0),
                                          "Not an engine",
                                          VK_MAKE_API_VERSION(0, 1, 0, 0))}
#if !defined(NDEBUG)
      ,
      vkDebugUtilsMessenger_{vkInstance_, graphic::makeDebugUtilsMessengerCreateInfoEXT()}
#endif
{
    vkPhysicalDevicePtr_ = std::make_unique<vk::raii::PhysicalDevice>(graphic::createPhysicalDevice(vkInstance_));
    vkDevicePtr_ = std::make_unique<vk::raii::Device>(graphic::createDevice(*vkPhysicalDevicePtr_));
}

} // namespace nae
