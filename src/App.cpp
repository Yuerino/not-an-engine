#include "App.hpp"

#include <GLFW/glfw3.h>

namespace nae {

App::App() : window_{WIDTH, HEIGHT, TITLE} {}

void App::run() {
    while (!window_.shouldClose()) {
        glfwPollEvents();
    }
}

} // namespace nae
