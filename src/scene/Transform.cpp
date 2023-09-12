#include "scene/Transform.hpp"

namespace nae {

Transform::Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
    : position_{position}, rotation_{rotation}, scale_{scale} {}

glm::mat4 Transform::getWorldMatrix() const {
    glm::mat4 rotMat = glm::toMat4(glm::quat{rotation_});

    return glm::translate(glm::mat4{1.0f}, position_) * rotMat * glm::scale(glm::mat4{1.0f}, scale_);
}

} // namespace nae
