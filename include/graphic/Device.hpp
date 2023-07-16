#ifndef NOT_AN_ENGINE_GRAPHIC_DEVICE_HPP
#define NOT_AN_ENGINE_GRAPHIC_DEVICE_HPP

#include <string>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

[[nodiscard]] vk::raii::Device createDevice(const vk::raii::PhysicalDevice &physicalDevice,
                                            uint32_t graphicQueueFamilyIndex,
                                            const std::vector<std::string> &extensions = {});

[[nodiscard]] std::pair<uint32_t, uint32_t>
findGraphicAndPresentQueueFamilyIndex(const vk::raii::PhysicalDevice &physicalDevice,
                                      const vk::raii::SurfaceKHR &surface);

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_DEVICE_HPP
