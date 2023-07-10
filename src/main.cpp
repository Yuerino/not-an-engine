#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>

#include "App.hpp"

int main() {
    glfwSetErrorCallback([](int error, const char *description) {
        std::cerr << "GLFW error " << error << ": " << description << std::endl;
    });

    try {
        nae::App app{};
        app.run();
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
