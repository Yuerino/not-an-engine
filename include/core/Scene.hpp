#pragma once

#include <cstdint>

#include "core/Camera.hpp"
#include "core/Entity.hpp"
#include "core/Time.hpp"
#include "renderer/Renderer.hpp"

namespace nae {

class Scene {
public:
    Scene();
    virtual ~Scene() = default;

    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;
    Scene(Scene &&) = default;
    Scene &operator=(Scene &&) = default;

    virtual void onAttach() = 0;
    virtual void onResize(float aspectRatio) = 0;
    virtual void onUpdate(Time timestep) = 0;

protected:
    Renderer *pRenderer_;
};

class BasicScene : public Scene {
public:
    BasicScene();

    void onAttach() override;
    void onResize(float aspectRatio) override;
    void onUpdate(Time timestep) override;

private:
    std::shared_ptr<Camera> pCamera_;
    std::unique_ptr<CameraController> pCameraController_;
    std::vector<std::unique_ptr<Entity>> entities_;
};

} // namespace nae
