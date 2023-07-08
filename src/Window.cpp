#include "Window.hpp"

namespace nae {
    Window::Window(int width, int height, std::string title) : width_{width}, height_{height}, title_{std::move(title)} {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
    }

    Window::~Window() {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(window_);
    }
}// namespace nae
