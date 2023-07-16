#include "graphic/Graphic.hpp"

#include "graphic/Device.hpp"
#include "graphic/Instance.hpp"
#include "graphic/PhysicalDevice.hpp"
#include "graphic/Surface.hpp"

namespace nae {

Graphic::Graphic(const Window &window)
    : vkContext_{},
      vkInstance_{graphic::createInstance(vkContext_,
                                          "Vulkan window",
                                          VK_MAKE_API_VERSION(0, 1, 0, 0),
                                          "Not an engine",
                                          VK_MAKE_API_VERSION(0, 1, 0, 0))},
      graphicQueueFamilyIndex_{0},
      presentQueueFamilyIndex_{0}
#if !defined(NDEBUG)
      ,
      vkDebugUtilsMessenger_{vkInstance_, graphic::makeDebugUtilsMessengerCreateInfoEXT()}
#endif
{
    vkPhysicalDevicePtr_ = std::make_unique<vk::raii::PhysicalDevice>(graphic::createPhysicalDevice(vkInstance_));

    vkSurfacePtr_ = std::make_unique<vk::raii::SurfaceKHR>(graphic::createSurface(vkInstance_, window.glfwWindow_));

    std::tie(graphicQueueFamilyIndex_, presentQueueFamilyIndex_) =
            graphic::findGraphicAndPresentQueueFamilyIndex(*vkPhysicalDevicePtr_, *vkSurfacePtr_);
    vkDevicePtr_ = std::make_unique<vk::raii::Device>(
            graphic::createDevice(*vkPhysicalDevicePtr_, graphicQueueFamilyIndex_, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}));

    vkCommandPoolPtr_ = std::make_unique<vk::raii::CommandPool>(
            *vkDevicePtr_,
            vk::CommandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphicQueueFamilyIndex_});

    vkGraphicQueuePtr_ = std::make_unique<vk::raii::Queue>(*vkDevicePtr_, graphicQueueFamilyIndex_, 0);
    vkPresentQueuePtr_ = std::make_unique<vk::raii::Queue>(*vkDevicePtr_, presentQueueFamilyIndex_, 0);

    SwapChainDataPtr_ = std::make_unique<graphic::SwapChainData>(
            *vkPhysicalDevicePtr_,
            *vkSurfacePtr_,
            *vkDevicePtr_,
            window.extent_,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
            graphicQueueFamilyIndex_,
            presentQueueFamilyIndex_);
}

} // namespace nae
