#pragma once

#include "scene/Behaviour.hpp"

namespace nae {

class FreeCameraController final : public Behaviour {
public:
    explicit FreeCameraController(float cameraMovSpeed = 5.0f, float cameraRotSpeed = 1.0f);

    void onUpdate(Time deltaTime) final;

private:
    float cameraMovSpeed_;
    float cameraRotSpeed_;
};

} // namespace nae
