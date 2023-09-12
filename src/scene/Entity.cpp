#include "scene/Entity.hpp"

#include <algorithm>
#include <cassert>

namespace nae {

Entity::Entity(std::string name) : name_{std::move(name)} {}

Component *Entity::getComponent(EComponentType componentType) const {
    for (const auto &component: components_) {
        if (component->getType() == componentType) {
            return component.get();
        }
    }

    return nullptr;
}

std::vector<Component *> Entity::getComponentOfType(EComponentType componentType) const {
    std::vector<Component *> componentOfType{};

    std::for_each(components_.begin(),
                  components_.end(),
                  [componentType, &componentOfType](const auto &component) -> void {
                      if (component->getType() == componentType) {
                          componentOfType.emplace_back(component.get());
                      }
                  });

    return componentOfType;
}

Component *Entity::addComponent(std::unique_ptr<Component> pComponent) {
    assert(pComponent != nullptr && "Trying to add empty component");

    pComponent->setEntity(this);

    const auto &pAddedComponent = components_.emplace_back(std::move(pComponent));
    return pAddedComponent.get();
}

bool Entity::removeComponent(Component *pComponentToRemove) {
    assert(pComponentToRemove != nullptr && "Trying to remove empty component");

    auto newEndIt = std::remove_if(
            components_.begin(),
            components_.end(),
            [pComponentToRemove](const auto &pComponent) -> bool { return pComponent.get() == pComponentToRemove; });

    if (newEndIt == components_.end()) {
        return false;
    }

    components_.erase(newEndIt, components_.end());
    return true;
}
bool Entity::removeComponent(EComponentType componentType) {
    auto newEndIt = std::remove_if(
            components_.begin(),
            components_.end(),
            [componentType](const auto &pComponent) -> bool { return pComponent->getType() == componentType; });

    if (newEndIt == components_.end()) {
        return false;
    }

    components_.erase(newEndIt, components_.end());
    return true;
}

} // namespace nae
