#pragma once

#include <GLFW/glfw3.h>

#include <string>

namespace nae {
    class Window {
    public:
        Window(int width, int height, std::string title);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        Window(Window &&) = delete;
        Window &operator=(Window &&) = delete;

        [[nodiscard]] bool shouldClose() const;

    private:
        GLFWwindow *window_;
        std::string title_;
        const int width_;
        const int height_;
    };
}// namespace nae
