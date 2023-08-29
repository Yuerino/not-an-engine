#include "core/Camera.hpp"

#include <cmath>
#include <utility>

#include <glfw/glfw3.h>

#include "core/App.hpp"

namespace nae {

Camera::Camera(float aspect, float fov, float nearPlane, float farPlane)
    : fov_{fov}, nearPlane_{nearPlane}, farPlane_{farPlane} {
    setAspect(aspect);
}

void Camera::setViewMatrix(const glm::vec3 &position, const glm::vec3 &forwardDir) noexcept {
    viewMatrix_ = glm::lookAt(position, position + forwardDir, glm::vec3{0.0f, 1.0f, 0.0f});
}

void Camera::setAspect(float aspect) noexcept {
    projectionMatrix_ = glm::perspective(fov_, aspect, nearPlane_, farPlane_);
    projectionMatrix_[1][1] *= -1;
}

CameraController::CameraController(std::shared_ptr<Camera> pCamera, float cameraMovSpeed, float cameraRotSpeed)
    : pCamera_{std::move(pCamera)}, cameraMovSpeed_{cameraMovSpeed}, cameraRotSpeed_{cameraRotSpeed} {}

void CameraController::onUpdate(Time deltaTime) noexcept {
    glm::vec3 rotation{0.0f};

    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_UP) == GLFW_PRESS) {
        rotation.x += 1.0f;
    }
    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_DOWN) == GLFW_PRESS) {
        rotation.x -= 1.0f;
    }

    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_LEFT) == GLFW_PRESS) {
        rotation.y -= 1.0f;
    }
    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_RIGHT) == GLFW_PRESS) {
        rotation.y += 1.0f;
    }

    if (glm::length(rotation) != 0.0f) {
        cameraRotation_ += glm::normalize(rotation) * static_cast<float>(deltaTime) * cameraRotSpeed_;
        cameraRotation_.x = glm::clamp(cameraRotation_.x, -glm::half_pi<float>(), glm::half_pi<float>());
        cameraRotation_.y = glm::mod(cameraRotation_.y, glm::two_pi<float>());
    }

    glm::vec3 forwardDir;
    forwardDir.x = cosf(cameraRotation_.x) * cosf(cameraRotation_.y);
    forwardDir.y = sinf(cameraRotation_.x);
    forwardDir.z = cosf(cameraRotation_.x) * sinf(cameraRotation_.y);
    glm::vec3 upDir{0.0f, 1.0f, 0.0f};
    glm::vec3 rightDir{glm::normalize(glm::cross(forwardDir, upDir))};

    glm::vec3 translation{0.f};
    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_W) == GLFW_PRESS) {
        translation += forwardDir;
    }
    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_S) == GLFW_PRESS) {
        translation -= forwardDir;
    }

    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_D) == GLFW_PRESS) {
        translation += rightDir;
    }
    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_A) == GLFW_PRESS) {
        translation -= rightDir;
    }

    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_SPACE) == GLFW_PRESS) {
        translation += upDir;
    }
    if (glfwGetKey(static_cast<GLFWwindow *>(App::get().getWindow()), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        translation -= upDir;
    }

    if (glm::length(translation) != 0.0f) {
        cameraPosition_ += glm::normalize(translation) * static_cast<float>(deltaTime) * cameraMovSpeed_;
    }

    pCamera_->setViewMatrix(cameraPosition_, forwardDir);
}

void CameraController::onResize(float aspect) noexcept {
    pCamera_->setAspect(aspect);
}

} // namespace nae
