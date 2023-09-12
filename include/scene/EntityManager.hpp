#pragma once

#include <algorithm>

#include "Component.hpp"
#include "Entity.hpp"

namespace nae {

class EntityManager {
private:
    using TEntities = std::vector<std::unique_ptr<Entity>>;

public:
    [[nodiscard]] Entity *getEntity(const std::string &name) const;

    Entity *addEntity(std::unique_ptr<Entity> pEntity);

    bool removeEntity(Entity *pEntityToRemove);

    [[nodiscard]] std::vector<Component *> queryComponentOfType(EComponentType componentType) const;

private:
    TEntities entities_{};
};

} // namespace nae
