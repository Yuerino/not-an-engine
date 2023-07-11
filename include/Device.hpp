#ifndef NOT_AN_ENGINE_DEVICE_HPP
#define NOT_AN_ENGINE_DEVICE_HPP

#include "vulkan/vulkan_raii.hpp"

namespace nae {

class Device {
public:
    Device();
    ~Device() = default;

    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;

private:
    vk::raii::Context vkContext_;
    vk::raii::Instance vkInstance_;
    vk::raii::PhysicalDevice vkPhysicalDevice_;
    vk::raii::Device vkDevice_;

#ifndef NDEBUG
    vk::raii::DebugUtilsMessengerEXT vkDebugUtilsMessenger_;
#endif
};

} // namespace nae

#endif // NOT_AN_ENGINE_DEVICE_HPP
