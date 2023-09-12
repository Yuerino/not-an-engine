#include "scene/Camera.hpp"

#include <cmath>

#include "core/App.hpp"

namespace nae {

Camera::Camera(float fov, float nearPlane, float farPlane) : fov_{fov}, nearPlane_{nearPlane}, farPlane_{farPlane} {
    calculateProjectionMatrix();
}

void Camera::calculateViewMatrix(glm::vec3 position, glm::vec3 rotation) {
    glm::vec3 upDir{0.0f, 1.0f, 0.0f};

    glm::vec3 forwardDir;
    forwardDir.x = cosf(rotation.x) * cosf(rotation.y);
    forwardDir.y = sinf(rotation.x);
    forwardDir.z = cosf(rotation.x) * sinf(rotation.y);

    glm::vec3 rightDir{glm::normalize(glm::cross(forwardDir, upDir))};

    viewMatrix_ = glm::lookAt(position, position + forwardDir, upDir);
}

void Camera::calculateProjectionMatrix() {
    float aspect = App::get().getWindow().getAspectRatio();
    projectionMatrix_ = glm::perspective(fov_, aspect, nearPlane_, farPlane_);
    projectionMatrix_[1][1] *= -1;
}

} // namespace nae
