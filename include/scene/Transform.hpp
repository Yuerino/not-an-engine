#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Component.hpp"

namespace nae {

class Transform final : public Component {
public:
    explicit Transform(const glm::vec3 &position = glm::vec3{0.0f},
                       const glm::vec3 &rotation = glm::vec3{0.0f},
                       const glm::vec3 &scale = glm::vec3{1.0f, 1.0f, 1.0f});

    [[nodiscard]] EComponentType getType() const noexcept final { return EComponentType::Transform; }

    [[nodiscard]] glm::mat4 getWorldMatrix() const;

    void setPosition(const glm::vec3 &position) { position_ = position; }
    void setRotation(const glm::vec3 &rotation) { rotation_ = rotation; }
    void setScale(const glm::vec3 &scale) { scale_ = scale; }

    [[nodiscard]] glm::vec3 getPosition() const noexcept { return position_; }
    [[nodiscard]] glm::vec3 getRotation() const noexcept { return rotation_; }
    [[nodiscard]] glm::vec3 getScale() const noexcept { return scale_; }

private:
    glm::vec3 position_;
    glm::vec3 rotation_;
    glm::vec3 scale_;
};

} // namespace nae
