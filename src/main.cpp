#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.hpp>

#include "core/App.hpp"
#include "core/GlfwApi.hpp"
#include "core/util.hpp"
#include "scene/BasicScene.hpp"

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
        app.setActiveScene<nae::BasicScene>();
        app.run();
    } catch (const nae::GlfwException &ex) {
        std::cerr << "GLFW error code: " << ex.getErrorCode() << ", description: " << ex.what() << '\n';
        return EXIT_FAILURE;
    } catch (const vk::Error &ex) {
        std::cerr << "Vulkan error: " << ex.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << '\n';
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception" << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
