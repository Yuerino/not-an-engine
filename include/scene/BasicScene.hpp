#pragma once

#include "Scene.hpp"

namespace nae {

class BasicScene final : public Scene {
public:
    BasicScene();

    void onAttach() final;

    void onUpdate(Time timestep) final;
};

} // namespace nae
