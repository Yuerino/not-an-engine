#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>

#include "App.hpp"

int main() {
    try {
        nae::App app{};
        app.run();
    } catch (const nae::GlfwException &ex) {
        std::cerr << "GLFW Error code: " << ex.getErrorCode() << ", description: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
