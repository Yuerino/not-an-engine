#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/Time.hpp"

namespace nae {

class Camera {
public:
    explicit Camera(float fov = 45.0f, float nearPlane = 0.1f, float farPlane = 100.0f);
    virtual ~Camera() = default;

    virtual void onUpdate(Time timestep) = 0;

    void calculateViewMatrix(glm::vec3 position, glm::vec3 rotation);

    void calculateProjectionMatrix();

    [[nodiscard]] float getFov() const noexcept { return fov_; }
    [[nodiscard]] float getNearPlane() const noexcept { return nearPlane_; }
    [[nodiscard]] float getFarPlane() const noexcept { return farPlane_; }
    [[nodiscard]] const glm::mat4 &getViewMatrix() const noexcept { return viewMatrix_; }
    [[nodiscard]] const glm::mat4 &getProjectionMatrix() const noexcept { return projectionMatrix_; }

private:
    float fov_;
    float nearPlane_;
    float farPlane_;

    glm::mat4 viewMatrix_{1.0f};
    glm::mat4 projectionMatrix_{1.0f};
};

} // namespace nae
