#include "scene/FreeCameraController.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/App.hpp"
#include "scene/Transform.hpp"

namespace nae {

FreeCameraController::FreeCameraController(float cameraMovSpeed, float cameraRotSpeed)
    : cameraMovSpeed_{cameraMovSpeed}, cameraRotSpeed_{cameraRotSpeed} {}

void FreeCameraController::onUpdate(Time deltaTime) {
    auto *pCameraTransformComponent = getEntity()->getComponent(EComponentType::Transform);
    auto *pCameraTransform = static_cast<Transform *>(pCameraTransformComponent);

    //! Camera rotation
    auto cameraRotation = pCameraTransform->getRotation();
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
        cameraRotation += glm::normalize(rotation) * static_cast<float>(deltaTime) * cameraRotSpeed_;
        cameraRotation.x = glm::clamp(cameraRotation.x, -glm::half_pi<float>(), glm::half_pi<float>());
        cameraRotation.y = glm::mod(cameraRotation.y, glm::two_pi<float>());

        pCameraTransform->setRotation(cameraRotation);
    }

    //! Camera movement
    // TODO: refactor
    glm::vec3 upDir{0.0f, 1.0f, 0.0f};

    glm::vec3 forwardDir;
    forwardDir.x = cosf(cameraRotation.x) * cosf(cameraRotation.y);
    forwardDir.y = sinf(cameraRotation.x);
    forwardDir.z = cosf(cameraRotation.x) * sinf(cameraRotation.y);

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
        auto cameraPosition = pCameraTransform->getPosition();
        cameraPosition += glm::normalize(translation) * static_cast<float>(deltaTime) * cameraMovSpeed_;
        pCameraTransform->setPosition(cameraPosition);
    }
}

} // namespace nae
