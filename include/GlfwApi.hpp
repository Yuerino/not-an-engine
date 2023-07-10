#pragma once

#include <string>

#include <GLFW/glfw3.h>

namespace nae {

class GlfwApi {
public:
    GlfwApi();
    ~GlfwApi();

    GlfwApi(const GlfwApi &) = delete;
    GlfwApi &operator=(const GlfwApi &) = delete;
    GlfwApi(GlfwApi &&) = delete;
    GlfwApi &operator=(GlfwApi &&) = delete;

private:
    static void errorCallback(int error, const char *description);
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

} // namespace nae
