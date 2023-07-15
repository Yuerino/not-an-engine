#ifndef NOT_AN_ENGINE_GRAPHIC_DEVICE_HPP
#define NOT_AN_ENGINE_GRAPHIC_DEVICE_HPP

#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

vk::raii::Device createDevice(const vk::raii::PhysicalDevice &physicalDevice);

}

#endif // NOT_AN_ENGINE_GRAPHIC_DEVICE_HPP
