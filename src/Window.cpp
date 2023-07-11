#include "Window.hpp"

#include <cassert>

#include "GlfwApi.hpp"

namespace nae {

Window::Window(int width, int height, std::string title) : width_{width}, height_{height}, title_{std::move(title)} {
    if (glfwVulkanSupported() == GLFW_FALSE) {
        throw GlfwException{-1, "Vulkan is not supported"};
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    glfwWindow_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
    assert(glfwWindow_ != nullptr);
}

Window::~Window() {
    glfwDestroyWindow(glfwWindow_);
}

bool Window::shouldClose() const noexcept {
    return glfwWindowShouldClose(glfwWindow_);
}

} // namespace nae
