#include "scene/Scene.hpp"

namespace nae {

Scene::Scene(std::unique_ptr<Camera> pCamera) : pCamera_{std::move(pCamera)} {}

} // namespace nae
