#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Component.hpp"

namespace nae {

enum class EComponentType;
class Component;

class Entity {
private:
    using TComponents = std::vector<std::unique_ptr<Component>>;

public:
    Entity() = default;
    explicit Entity(std::string name);

    //! Get component

    [[nodiscard]] Component *getComponent(EComponentType componentType) const;

    [[nodiscard]] std::vector<Component *> getComponentOfType(EComponentType componentType) const;

    //! Add component

    Component *addComponent(std::unique_ptr<Component> pComponent);

    template<typename ComponentType, typename... Args>
        requires std::derived_from<ComponentType, Component>
    ComponentType *addComponent(Args &&...args) {
        return static_cast<ComponentType *>(addComponent(std::make_unique<ComponentType>(std::forward<Args>(args)...)));
    }

    //! Remove component

    bool removeComponent(Component *pComponentToRemove);

    bool removeComponent(EComponentType componentType);

    //! Getters and setters

    void setName(const std::string &name) { name_ = name; }

    [[nodiscard]] const std::string &getName() const noexcept { return name_; }
    [[nodiscard]] const TComponents &getComponents() const noexcept { return components_; }

private:
    std::string name_;
    TComponents components_{};
};

} // namespace nae
