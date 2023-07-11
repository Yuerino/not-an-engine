#include "App.hpp"

#include <GLFW/glfw3.h>

#include "util/vulkan.hpp"

namespace nae {

App::App() : glfwApi_{}, vkContext_{} {
    if (glfwVulkanSupported() == GLFW_FALSE) {
        throw GlfwException{-1, "Vulkan is not supported"};
    }

    vkInstancePtr_ = std::make_unique<vk::raii::Instance>(util::vk::createInstance(vkContext_,
                                                                                   "Vulkan window",
                                                                                   VK_MAKE_API_VERSION(0, 1, 0, 0),
                                                                                   "Not an engine",
                                                                                   VK_MAKE_API_VERSION(0, 1, 0, 0)));

#ifndef NDEBUG
    vkDebugUtilsMessengerPtr_ =
            std::make_unique<vk::raii::DebugUtilsMessengerEXT>(*vkInstancePtr_,
                                                               util::vk::makeDebugUtilsMessengerCreateInfoEXT());
#endif

    windowPtr_ = std::make_unique<Window>(WIDTH, HEIGHT, TITLE);
}

void App::run() {
    while (!windowPtr_->shouldClose()) {
        glfwPollEvents();
    }
}

} // namespace nae
