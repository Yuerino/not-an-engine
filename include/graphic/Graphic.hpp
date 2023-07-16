#ifndef NOT_AN_ENGINE_GRAPHIC_GRAPHIC_HPP
#define NOT_AN_ENGINE_GRAPHIC_GRAPHIC_HPP

#include <memory>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

#include "Window.hpp"
#include "graphic/SwapChainData.hpp"

namespace nae {

class Graphic {
public:
    explicit Graphic(const Window &window);
    ~Graphic() = default;

    Graphic(const Graphic &) = delete;
    Graphic &operator=(const Graphic &) = delete;
    Graphic(Graphic &&) = delete;
    Graphic &operator=(Graphic &&) = delete;

private:
    vk::raii::Context vkContext_;
    vk::raii::Instance vkInstance_;
    std::unique_ptr<vk::raii::PhysicalDevice> vkPhysicalDevicePtr_;
    std::unique_ptr<vk::raii::SurfaceKHR> vkSurfacePtr_;
    std::unique_ptr<vk::raii::Device> vkDevicePtr_;
    std::unique_ptr<vk::raii::CommandPool> vkCommandPoolPtr_;
    std::unique_ptr<vk::raii::Queue> vkGraphicQueuePtr_;
    std::unique_ptr<vk::raii::Queue> vkPresentQueuePtr_;
    std::unique_ptr<graphic::SwapChainData> SwapChainDataPtr_;

    uint32_t graphicQueueFamilyIndex_;
    uint32_t presentQueueFamilyIndex_;

#if !defined(NDEBUG)
    vk::raii::DebugUtilsMessengerEXT vkDebugUtilsMessenger_;
#endif
};

} // namespace nae

#endif // NOT_AN_ENGINE_GRAPHIC_GRAPHIC_HPP
