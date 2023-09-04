#include "core/Scene.hpp"

#include "vulkan/vulkan.hpp"

#include "core/App.hpp"

namespace nae {

Scene::Scene() : pRenderer_{&App::get().getRenderer()} {}

BasicScene::BasicScene()
    : pCamera_{std::make_shared<Camera>(App::get().getWindow().getAspectRatio())},
      pCameraController_{std::make_unique<CameraController>(pCamera_)} {
    // 0 : floor
    entities_.emplace_back(std::make_unique<Entity>(
            std::make_unique<Model>("C:/Users/yueri/Documents/Project/not-an-engine/model/floor.obj")));
    pFloorTexture_ = std::make_unique<Texture>(vk::Extent2D{1024, 1024});
    pFloorTexture_->setTexels("C:/Users/yueri/Documents/Project/not-an-engine/model/PavingStones135_1K.png");

    // 1: 42 Obj
    entities_.emplace_back(std::make_unique<Entity>(
            std::make_unique<Model>("C:/Users/yueri/Documents/Project/not-an-engine/model/forty_two.obj")));
    p42Texture_ = std::make_unique<Texture>(vk::Extent2D{1024, 1024});
    p42Texture_->setTexels("C:/Users/yueri/Documents/Project/not-an-engine/model/Concrete042A_1K.png");

    // 2 : vase
    entities_.emplace_back(std::make_unique<Entity>(
            std::make_unique<Model>("C:/Users/yueri/Documents/Project/not-an-engine/model/vase.obj")));
    pVaseTexture_ = std::make_unique<Texture>(vk::Extent2D{1024, 1024});
    pVaseTexture_->setTexels("C:/Users/yueri/Documents/Project/not-an-engine/model/Wood054_1K.png");
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
    pRenderer_->draw(*entities_[0], *pFloorTexture_);
    pRenderer_->draw(*entities_[1], *p42Texture_);
    pRenderer_->draw(*entities_[2], *pVaseTexture_);
    pRenderer_->endScene();
}

} // namespace nae
