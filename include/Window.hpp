#ifndef NOT_AN_ENGINE_WINDOW_HPP
#define NOT_AN_ENGINE_WINDOW_HPP

#include <string>

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
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    [[nodiscard]] bool shouldClose() const noexcept;

private:
    GLFWwindow *glfwWindow_;
    vk::Extent2D extent_;
    std::string title_;
};

} // namespace nae

#endif // NOT_AN_ENGINE_WINDOW_HPP
