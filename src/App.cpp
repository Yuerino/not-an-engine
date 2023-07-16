#include "App.hpp"

namespace nae {

App::App() : glfwApi_{}, window_{WINDOW_EXTENT, TITLE}, graphic_{window_} {}

void App::run() {
    while (!window_.shouldClose()) {
        glfwWrapper([]() { glfwPollEvents(); });
    }
}

} // namespace nae
