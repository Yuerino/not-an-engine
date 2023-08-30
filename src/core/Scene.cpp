#include "core/Scene.hpp"

#include "core/App.hpp"

namespace nae {

Scene::Scene() : pRenderer_{&App::get().getRenderer()} {}

BasicScene::BasicScene()
    : pCamera_{std::make_shared<Camera>(App::get().getWindow().getAspectRatio())},
      pCameraController_{std::make_unique<CameraController>(pCamera_)} {
    // 0 : floor
    entities_.emplace_back(std::make_unique<Entity>(
            std::make_unique<Model>("C:/Users/yueri/Documents/Project/not-an-engine/model/floor.obj")));
    // 1: 42 Obj
    entities_.emplace_back(std::make_unique<Entity>(
            std::make_unique<Model>("C:/Users/yueri/Documents/Project/not-an-engine/model/forty_two.obj")));
    // 2 : vase
    entities_.emplace_back(std::make_unique<Entity>(
            std::make_unique<Model>("C:/Users/yueri/Documents/Project/not-an-engine/model/vase.obj")));
}

void BasicScene::onAttach() {
    auto &floorEntity = *entities_[0];
    auto floorEntityTrans = floorEntity.getTransform();
    floorEntityTrans.scale = glm::vec3(10.0f, 10.0f, 10.0f);
    floorEntity.setTransform(floorEntityTrans);

    auto &ftEntity = *entities_[1];
    auto ftEntityTrans = ftEntity.getTransform();
    ftEntityTrans.translation = glm::vec3(5.0f, 0.0f, 0.0f);
    ftEntity.setTransform(ftEntityTrans);

    auto &vaseEntity = *entities_[2];
    auto vaseEntityTrans = vaseEntity.getTransform();
    vaseEntityTrans.scale = glm::vec3(5.0f, 5.0f, 5.0f);
    vaseEntity.setTransform(vaseEntityTrans);
}

void BasicScene::onResize(float aspectRatio) {
    pCameraController_->onResize(aspectRatio);
}

void BasicScene::onUpdate(Time timestep) {
    pCameraController_->onUpdate(timestep);
    pRenderer_->beginScene(*pCamera_);
    for (auto &entity: entities_) {
        if (entity->hasModel()) {
            pRenderer_->draw(*entity);
        }
    }
    pRenderer_->endScene();
}

} // namespace nae
