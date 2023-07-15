#ifndef NOT_AN_ENGINE_GRAPHIC_INSTANCE_HPP
#define NOT_AN_ENGINE_GRAPHIC_INSTANCE_HPP

#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

vk::DebugUtilsMessengerCreateInfoEXT makeDebugUtilsMessengerCreateInfoEXT();

vk::raii::Instance createInstance(vk::raii::Context &vkContext,
                                  const std::string &appName,
                                  uint32_t appVersion,
                                  const std::string &engineName,
                                  uint32_t engineVersion,
                                  const std::vector<std::string> &layers = {},
                                  const std::vector<std::string> &extensions = {},
                                  uint32_t apiVersion = VK_API_VERSION_1_3);

} // namespace nae::graphic


#endif // NOT_AN_ENGINE_GRAPHIC_INSTANCE_HPP