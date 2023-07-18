#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace nae {

class Window {
    friend class Graphic;

public:
    Window(vk::Extent2D extent, std::string title);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    [[nodiscard]] bool shouldClose() const noexcept;

    void createVulkanSurface(const VkInstance &vkInstance,
                             const VkAllocationCallbacks *allocator,
                             VkSurfaceKHR *surface) const;

private:
    GLFWwindow *glfwWindow_;
    vk::Extent2D extent_;
    std::string title_;
};

} // namespace nae
