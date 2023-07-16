#include "graphic/Surface.hpp"

#include <vulkan/vulkan.hpp>

#include "GlfwApi.hpp"

namespace nae::graphic {

vk::raii::SurfaceKHR createSurface(vk::raii::Instance &vkInstance, GLFWwindow *glfwWindow) {
    VkSurfaceKHR surface;

    auto result = glfwWrapper([&]() {
        return glfwCreateWindowSurface(static_cast<VkInstance>(*vkInstance), glfwWindow, nullptr, &surface);
    });
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }

    return vk::raii::SurfaceKHR{vkInstance, surface};
}

} // namespace nae::graphic
