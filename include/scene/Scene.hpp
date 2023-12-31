#pragma once

#include "Camera.hpp"
#include "EntityManager.hpp"
#include "core/Time.hpp"

namespace nae {

class Scene {
public:
    explicit Scene(std::unique_ptr<Camera> pCamera) : pCamera_{std::move(pCamera)} {}
    virtual ~Scene() = default;

    virtual void onAttach() = 0;
    virtual void onUpdate(Time timestep) = 0;

    [[nodiscard]] EntityManager &getEntityManager() noexcept { return entityManager_; }
    [[nodiscard]] Camera *getCamera() const noexcept { return pCamera_.get(); }

private:
    std::unique_ptr<Camera> pCamera_;
    EntityManager entityManager_;
};

} // namespace nae
