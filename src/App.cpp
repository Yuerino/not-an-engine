#include "App.hpp"

#include <GLFW/glfw3.h>

namespace nae {

App::App() : glfwApi_{} {
    if (glfwVulkanSupported() == GLFW_FALSE) {
        throw GlfwException{-1, "Vulkan is not supported"};
    }

    window_ = std::make_unique<Window>(WIDTH, HEIGHT, TITLE);
}

void App::run() {
    while (!window_->shouldClose()) {
        glfwPollEvents();
    }
}

} // namespace nae
