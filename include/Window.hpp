#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace nae {

class Window {
public:
    Window(vk::Extent2D extent, std::string title);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    [[nodiscard]] bool shouldClose() const noexcept;

    void createVulkanSurface(const VkInstance &vkInstance,
                             const VkAllocationCallbacks *allocator,
                             VkSurfaceKHR *surface) const;

    [[nodiscard]] const vk::Extent2D &getExtent() const noexcept;
    [[nodiscard]] bool isFramebufferResized() const noexcept;
    void resetFramebufferResized() noexcept;

private:
    static void framebufferResizeCallback(GLFWwindow *glfwWindow, int width, int height);

    GLFWwindow *glfwWindow_;
    vk::Extent2D extent_;
    std::string title_;
    bool framebufferResized_ = false;
};

} // namespace nae
