#ifndef NOT_AN_ENGINE_GRAPHIC_HPP
#define NOT_AN_ENGINE_GRAPHIC_HPP

#include <memory>

#include "vulkan/vulkan_raii.hpp"

namespace nae {

class Graphic {
public:
    Graphic();
    ~Graphic() = default;

    Graphic(const Graphic &) = delete;
    Graphic &operator=(const Graphic &) = delete;
    Graphic(Graphic &&) = delete;
    Graphic &operator=(Graphic &&) = delete;

private:
    vk::raii::Context vkContext_;
    vk::raii::Instance vkInstance_;
    std::unique_ptr<vk::raii::PhysicalDevice> vkPhysicalDevicePtr_;
    std::unique_ptr<vk::raii::Device> vkDevicePtr_;

#if !defined(NDEBUG)
    vk::raii::DebugUtilsMessengerEXT vkDebugUtilsMessenger_;
#endif
};

} // namespace nae

#endif // NOT_AN_ENGINE_GRAPHIC_HPP
