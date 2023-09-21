#include "sample/BasicScene.hpp"

#include "sample/FreeCamera.hpp"
#include "sample/FreeCameraController.hpp"
#include "scene/Entity.hpp"
#include "scene/Model.hpp"
#include "scene/TextureMaterial.hpp"
#include "scene/Transform.hpp"

namespace nae {

BasicScene::BasicScene() : Scene{std::make_unique<FreeCamera>()} {}

void BasicScene::onAttach() {
    //! Camera controller
    auto *pCameraController = getEntityManager().addEntity(std::make_unique<Entity>("cameraController"));
    pCameraController->addComponent<Transform>(glm::vec3{0.0f, 5.0f, 5.0f},
                                               glm::vec3{0.0f, -glm::half_pi<float>(), 0.0f});
    pCameraController->addComponent<FreeCameraController>();

    //! Plane
    auto *pPlaneEntity = getEntityManager().addEntity(std::make_unique<Entity>("plane"));
    pPlaneEntity->addComponent<Model>(
            std::make_unique<Mesh>("resource/mesh/floor.obj"),
            std::make_unique<TextureMaterial>("paving stone", "resource/texture/PavingStones135_1K.png"));
    pPlaneEntity->addComponent<Transform>(glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{10.0f, 10.0f, 10.0f});

    //! 42 obj
    auto *p42Entity = getEntityManager().addEntity(std::make_unique<Entity>("42"));
    p42Entity->addComponent<Model>(
            std::make_unique<Mesh>("resource/mesh/forty_two.obj"),
            std::make_unique<TextureMaterial>("concrete", "resource/texture/Concrete042A_1K.png"));
    p42Entity->addComponent<Transform>(glm::vec3{5.0f, 0.0f, 0.0f});

    //! Vase obj
    auto *pVaseEntity = getEntityManager().addEntity(std::make_unique<Entity>("vase"));
    pVaseEntity->addComponent<Model>(std::make_unique<Mesh>("resource/mesh/vase.obj"),
                                     std::make_unique<TextureMaterial>("wood", "resource/texture/Wood054_1K.png"));
    pVaseEntity->addComponent<Transform>(glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{5.0f, 5.0f, 5.0f});
}


void BasicScene::onUpdate(Time timestep) {
    const auto &behaviourComponents = getEntityManager().queryComponentOfType(EComponentType::Behaviour);
    std::for_each(behaviourComponents.begin(), behaviourComponents.end(), [timestep](auto *pComponent) {
        auto *pBehaviour = static_cast<Behaviour *>(pComponent);
        pBehaviour->onUpdate(timestep);
    });

    getCamera().onUpdate(timestep);
}

} // namespace nae
