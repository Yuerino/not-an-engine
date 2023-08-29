#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/Time.hpp"

namespace nae {

class Camera {
public:
    explicit Camera(float aspect, float fov = 45.0f, float nearPlane = 0.1f, float farPlane = 100.f);
    ~Camera() = default;

    Camera(const Camera &) = delete;
    Camera &operator=(const Camera &) = delete;
    Camera(Camera &&) = default;
    Camera &operator=(Camera &&) = default;

    [[nodiscard]] const glm::mat4 &getViewMatrix() const noexcept { return viewMatrix_; }
    [[nodiscard]] const glm::mat4 &getProjectionMatrix() const noexcept { return projectionMatrix_; }

    void setViewMatrix(const glm::vec3 &position, const glm::vec3 &forwardDir) noexcept;
    void setAspect(float aspect) noexcept;

private:
    float fov_;
    float nearPlane_;
    float farPlane_;

    glm::mat4 viewMatrix_{1.0f};
    glm::mat4 projectionMatrix_{1.0f};
};

class CameraController {
public:
    explicit CameraController(std::shared_ptr<Camera> pCamera,
                              float cameraMovSpeed = 5.0f,
                              float cameraRotSpeed = 1.0f);
    ~CameraController() = default;

    CameraController(const CameraController &) = delete;
    CameraController &operator=(const CameraController &) = delete;
    CameraController(CameraController &&) = default;
    CameraController &operator=(CameraController &&) = default;

    void onUpdate(Time deltaTime) noexcept;
    void onResize(float aspect) noexcept;

    [[nodiscard]] Camera &getCamera() const noexcept { return *pCamera_; }

private:
    std::shared_ptr<Camera> pCamera_;
    float cameraMovSpeed_;
    float cameraRotSpeed_;

    glm::vec3 cameraRotation_{0.0f, -glm::half_pi<float>(), 0.0f};
    glm::vec3 cameraPosition_{0.0f, 0.0f, 5.0f};
};

} // namespace nae
