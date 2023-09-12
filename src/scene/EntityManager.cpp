#include "scene/EntityManager.hpp"

#include <cassert>

namespace nae {

Entity *EntityManager::getEntity(const std::string &name) const {
    for (const auto &pEntity: entities_) {
        if (pEntity->getName() == name) {
            return pEntity.get();
        }
    }

    return nullptr;
}

Entity *EntityManager::addEntity(std::unique_ptr<Entity> pEntity) {
    assert(pEntity != nullptr && "Trying to add empty entity");

    const auto &addedEntity = entities_.emplace_back(std::move(pEntity));

    return addedEntity.get();
}

bool EntityManager::removeEntity(Entity *pEntityToRemove) {
    assert(pEntityToRemove != nullptr && "Trying to remove empty entity");

    const auto &newEndIt = std::remove_if(
            entities_.begin(),
            entities_.end(),
            [pEntityToRemove](const auto &pEntity) -> bool { return pEntity.get() == pEntityToRemove; });

    if (newEndIt == entities_.end()) {
        return false;
    }

    entities_.erase(newEndIt, entities_.end());
    return true;
}

std::vector<Component *> EntityManager::queryComponentOfType(EComponentType componentType) const {
    std::vector<Component *> componentOfType{};

    std::for_each(entities_.begin(), entities_.end(), [componentType, &componentOfType](const auto &pEntity) -> void {
        auto pComponent = pEntity->getComponent(componentType);

        if (pComponent != nullptr) {
            componentOfType.emplace_back(pComponent);
        }
    });

    return componentOfType;
}

} // namespace nae
