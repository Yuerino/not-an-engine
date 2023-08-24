#include "core/GlfwApi.hpp"

namespace nae {

GlfwApi::GlfwApi() {
    glfwWrapper([]() { glfwInit(); });

    if (glfwVulkanSupported() == GLFW_FALSE) {
        throw GlfwException{-1, "Vulkan is not supported"};
    }
}

GlfwApi::~GlfwApi() {
    glfwTerminate();
}

GlfwException::GlfwException(int error, std::string description) noexcept
    : error_{error}, description_{std::move(description)} {}

int GlfwException::getErrorCode() const noexcept {
    return error_;
}

const char *GlfwException::what() const noexcept {
    return description_.c_str();
}

} // namespace nae
