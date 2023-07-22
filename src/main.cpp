#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.hpp>

#include "App.hpp"
#include "util.hpp"

int main() {
#ifdef __linux__
    nae::util::fragShaderPath = "/home/yuerino/project/not-an-engine/shader/shader.frag.spv";
    nae::util::vertShaderPath = "/home/yuerino/project/not-an-engine/shader/shader.vert.spv";
#elif _WIN32
    nae::util::fragShaderPath = "C:/Users/yueri/Documents/Project/not-an-engine/shader/shader.frag.spv";
    nae::util::vertShaderPath = "C:/Users/yueri/Documents/Project/not-an-engine/shader/shader.vert.spv";
#endif

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
