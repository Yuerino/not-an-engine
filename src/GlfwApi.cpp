#include "GlfwApi.hpp"

namespace nae {

GlfwApi::GlfwApi() {
    glfwSetErrorCallback(errorCallback);
    glfwInit();
}

GlfwApi::~GlfwApi() {
    glfwTerminate();
}

// TODO: when throwing in C callback, exception handler/unwind tables might not exist (system dependant). This might
//  lead to UB unless we built GLFW with same toolchain and statically link it and depends on compiler, we might have
//  to enable some options for this. All in all, this is a nightmare.
void GlfwApi::errorCallback(int error, const char *description) {
    throw GlfwException{error, description};
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
