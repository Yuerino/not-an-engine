#ifndef NOT_AN_ENGINE_GRAPHIC_PHYSICALDEVICE_HPP
#define NOT_AN_ENGINE_GRAPHIC_PHYSICALDEVICE_HPP

#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

vk::raii::PhysicalDevice createPhysicalDevice(const vk::raii::Instance &instance);

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_PHYSICALDEVICE_HPP
