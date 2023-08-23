#include "core/Window.hpp"

#include <cassert>

#include "core/GlfwApi.hpp"

namespace nae {

Window::Window(vk::Extent2D extent, std::string title) : extent_{extent}, title_{std::move(title)} {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    glfwWindow_ = glfwWrapper([&]() {
        return glfwCreateWindow(static_cast<int>(extent_.width),
                                static_cast<int>(extent_.height),
                                title_.c_str(),
                                nullptr,
                                nullptr);
    });
    assert(glfwWindow_ != nullptr && "Failed to create window");

    glfwWrapper([&]() { glfwSetWindowUserPointer(glfwWindow_, this); });
    glfwWrapper([&]() { glfwSetFramebufferSizeCallback(glfwWindow_, framebufferResizeCallback); });
}

Window::~Window() {
    glfwDestroyWindow(glfwWindow_);
}

bool Window::shouldClose() const noexcept {
    // TODO: add callback for close instead
    return static_cast<bool>(glfwWrapper([&]() { return glfwWindowShouldClose(glfwWindow_); }));
}

const vk::Extent2D &Window::getExtent() const noexcept {
    return extent_;
}

bool Window::isFramebufferResized() const noexcept {
    return framebufferResized_;
}

void Window::resetFramebufferResized() noexcept {
    framebufferResized_ = false;
}

void Window::framebufferResizeCallback(GLFWwindow *glfwWindow, int width, int height) {
    auto *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
    window->extent_.width = static_cast<uint32_t>(width);
    window->extent_.height = static_cast<uint32_t>(height);
    window->framebufferResized_ = true;
}

Window::operator GLFWwindow *() const noexcept {
    return glfwWindow_;
}

} // namespace nae
