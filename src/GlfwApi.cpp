#include "GlfwApi.hpp"

namespace nae {

GlfwApi::GlfwApi() {
    glfwWrapper([]() { glfwInit(); });
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
