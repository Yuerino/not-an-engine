#pragma once

#include <cstdint>

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
    virtual void onUpdate(Time timestep) = 0;

protected:
    Renderer *pRenderer_;
};

class BasicScene : public Scene {
public:
    BasicScene();

    void onAttach() override;
    void onUpdate(Time timestep) override;

private:
    std::unique_ptr<std::vector<Vertex>> pVertices_;
    std::unique_ptr<Buffer> pVertexBuffer_;
};

} // namespace nae
