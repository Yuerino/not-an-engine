#ifndef NOT_AN_ENGINE_GRAPHIC_SURFACE_HPP
#define NOT_AN_ENGINE_GRAPHIC_SURFACE_HPP

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

[[nodiscard]] vk::raii::SurfaceKHR createSurface(const vk::raii::Instance &vkInstance, GLFWwindow *glfwWindow);

[[nodiscard]] vk::SurfaceFormatKHR pickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &formats);

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_SURFACE_HPP
