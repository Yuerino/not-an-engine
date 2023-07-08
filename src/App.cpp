#include "App.hpp"

namespace nae {
    void App::run() {
        while (!window_.shouldClose()) {
            glfwPollEvents();
        }
    }
}// namespace nae
