#pragma once

#include "Camera.hpp"

namespace nae {

class FreeCamera final : public Camera {
public:
    FreeCamera() = default;

    void onUpdate(Time timestep) final;
};

} // namespace nae
