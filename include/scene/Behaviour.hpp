#pragma once

#include "Component.hpp"
#include "core/Time.hpp"

namespace nae {

class Behaviour : public Component {
public:
    ~Behaviour() override = default;

    [[nodiscard]] EComponentType getType() const noexcept override { return EComponentType::Behaviour; }

    virtual void onUpdate(Time timestep) = 0;
};

} // namespace nae
