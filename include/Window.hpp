#pragma once

#include <string>

#include <GLFW/glfw3.h>

namespace nae {

class Window {
public:
    Window(int width, int height, std::string title);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    [[nodiscard]] bool shouldClose() const noexcept;

private:
    GLFWwindow *glfwWindow_;
    const int width_;
    const int height_;
    std::string title_;
};

} // namespace nae
