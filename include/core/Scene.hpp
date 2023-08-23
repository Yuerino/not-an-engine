#pragma once

#include <cstdint>

#include "core/Time.hpp"
#include "renderer/Renderer.hpp"

namespace nae {

class Scene {
public:
    Scene() = default;
    virtual ~Scene() = default;

    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;
    Scene(Scene &&) = default;
    Scene &operator=(Scene &&) = default;

    virtual void onAttach(Renderer &renderer);
    virtual void onUpdate(Time timestep) = 0;

protected:
    Renderer *pRenderer_{nullptr};
};

class BasicScene : public Scene {
public:
    BasicScene();

    void onAttach(Renderer &renderer) override;
    void onUpdate(Time timestep) override;

private:
    std::unique_ptr<std::vector<Vertex>> pVertices_;
    std::unique_ptr<Buffer> pVertexBuffer_;
};

} // namespace nae
