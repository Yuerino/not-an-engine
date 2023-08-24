#ifndef NOT_AN_ENGINE_GLFW_API_HPP
#define NOT_AN_ENGINE_GLFW_API_HPP

#include <functional>
#include <string>

#include "GLFW/glfw3.h"

namespace nae {

class GlfwApi {
public:
    GlfwApi();
    ~GlfwApi();

    GlfwApi(const GlfwApi &) = delete;
    GlfwApi &operator=(const GlfwApi &) = delete;
    GlfwApi(GlfwApi &&) = default;
    GlfwApi &operator=(GlfwApi &&) = default;
};

class GlfwException : public std::exception {
public:
    /*!
     * @brief Constructs a GLFW exception.
     * @param error GLFW error code or -1 if the error is not from GLFW.
     * @param description Error description
     */
    GlfwException(int error, std::string description) noexcept;
    ~GlfwException() override = default;

    /*!
     * @brief Returns the exception error code.
     *
     * @return The exception error code that inherits from GLFW error code, or -1 if the error is not from GLFW.
     */
    [[nodiscard]] int getErrorCode() const noexcept;
    [[nodiscard]] const char *what() const noexcept override;

private:
    const int error_;
    std::string description_;
};

namespace {

    void checkGlfwError() {
        const char *description;
        int error = glfwGetError(&description);
        if (error != GLFW_NO_ERROR && description != nullptr) {
            throw GlfwException{error, description};
        }
    }

    template<typename F>
    concept FuncRetVoid = std::is_void_v<std::invoke_result_t<F>>;

    template<typename F>
    concept FuncRetNonVoid = !std::is_void_v<std::invoke_result_t<F>>;

} // namespace

/*!
 * @brief Wraps GLFW functions to checks for errors and throw them as exception safely
 *
 * @note Read issues #2
 */
template<typename F, typename Result = std::invoke_result_t<F>>
    requires FuncRetNonVoid<F>
Result glfwWrapper(F &&f) {
    Result result = std::invoke(std::forward<F>(f));
    checkGlfwError();
    return result;
}

/*!
 * @brief Wraps GLFW functions to checks for errors and throw them as exception safely
 *
 * @note Read issues #2
 */
template<typename F>
    requires FuncRetVoid<F>
void glfwWrapper(F &&f) {
    std::invoke(std::forward<F>(f));
    checkGlfwError();
}

} // namespace nae

#endif // NOT_AN_ENGINE_GLFW_API_HPP
