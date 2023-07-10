#include "App.hpp"

#include <GLFW/glfw3.h>

namespace nae {

App::App() : glfwApi_{}, vkContext_{}, window_{} {
    if (glfwVulkanSupported() == GLFW_FALSE) {
        throw GlfwException{-1, "Vulkan is not supported"};
    }

    // TODO: extract this whole vulkan stuff to a separate function
    vk::ApplicationInfo appInfo{"Vulkan Application",
                                VK_MAKE_VERSION(1, 0, 0),
                                "No Engine",
                                VK_MAKE_VERSION(1, 0, 0),
                                VK_API_VERSION_1_0};

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensionName = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    // TODO: checking if extensions are supported
    vk::InstanceCreateInfo instanceCreateInfo{{}, &appInfo, 0, {}, glfwExtensionCount, glfwExtensionName};
    vkInstance_ = std::make_unique<vk::raii::Instance>(vkContext_, instanceCreateInfo);

    window_ = std::make_unique<Window>(WIDTH, HEIGHT, TITLE);
}

void App::run() {
    while (!window_->shouldClose()) {
        glfwPollEvents();
    }
}

} // namespace nae
