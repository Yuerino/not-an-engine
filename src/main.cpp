#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "App.hpp"

int main() {
    try {
        nae::App app{};
        app.run();
    } catch (const nae::GlfwException &ex) {
        std::cerr << "GLFW error code: " << ex.getErrorCode() << ", description: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const vk::Error &ex) {
        std::cerr << "Vulkan error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
