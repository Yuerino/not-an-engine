#include "Window.hpp"

#include <stdexcept>

namespace nae {

Window::Window(int width, int height, std::string title) : width_{width}, height_{height}, title_{std::move(title)} {
    if (glfwInit() == GLFW_FALSE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    if (glfwVulkanSupported() == GLFW_FALSE) {
        glfwTerminate();
        throw std::runtime_error("Vulkan is not supported");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    glfwWindow_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
    if (glfwWindow_ == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
}

Window::~Window() {
    glfwDestroyWindow(glfwWindow_);
    glfwTerminate();
}

bool Window::shouldClose() const noexcept {
    return glfwWindowShouldClose(glfwWindow_);
}

} // namespace nae
