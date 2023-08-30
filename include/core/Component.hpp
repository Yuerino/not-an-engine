#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace nae {

struct TransformComponent {
    glm::vec3 translation{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    [[nodiscard]] glm::mat4 getTransform() const noexcept {
        glm::mat4 rotMat4{glm::toMat4(glm::quat{rotation})};
        return glm::translate(glm::mat4{1.0f}, translation) * rotMat4 * glm::scale(glm::mat4{1.0f}, scale);
    }
};

} // namespace nae
