#pragma once

#include "Entity.hpp"

namespace nae {

enum class EComponentType { Transform, Mesh, Behaviour };

class Entity;

class Component {
public:
    virtual ~Component() = default;

    [[nodiscard]] virtual EComponentType getType() const noexcept = 0;

    void setEntity(Entity *pEntity) noexcept { pEntity_ = pEntity; }

    [[nodiscard]] Entity *getEntity() const noexcept { return pEntity_; }

private:
    Entity *pEntity_{nullptr};
};

} // namespace nae
