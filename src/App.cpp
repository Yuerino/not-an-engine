#include "App.hpp"

namespace nae {

App::App() : glfwApi_{}, window_{WIDTH, HEIGHT, TITLE}, graphic_{} {}

void App::run() {
    while (!window_.shouldClose()) {
        glfwWrapper([]() { glfwPollEvents(); });
    }
}

} // namespace nae
