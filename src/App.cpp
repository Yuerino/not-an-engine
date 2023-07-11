#include "App.hpp"

namespace nae {

App::App() : glfwApi_{}, window_{WIDTH, HEIGHT, TITLE}, device_{} {}

void App::run() {
    while (!window_.shouldClose()) {
        glfwPollEvents();
    }
}

} // namespace nae
