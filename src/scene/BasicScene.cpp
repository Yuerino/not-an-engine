#include "scene/BasicScene.hpp"

#include "scene/Entity.hpp"
#include "scene/FreeCamera.hpp"
#include "scene/FreeCameraController.hpp"
#include "scene/Mesh.hpp"
#include "scene/Texture.hpp"
#include "scene/Transform.hpp"

namespace nae {

BasicScene::BasicScene() : Scene{std::make_unique<FreeCamera>()} {}

void BasicScene::onAttach() {
    //! Camera controller
    auto *pCameraController = getEntityManager().addEntity(std::make_unique<Entity>("cameraController"));
    pCameraController->addComponent<Transform>(glm::vec3{0.0f, 5.0f, 5.0f},
                                               glm::vec3{0.0f, -glm::half_pi<float>(), 0.0f});
    pCameraController->addComponent<FreeCameraController>();

    //! Paving Stones texture
    auto pPavingStonesTexture = std::make_unique<Texture>(vk::Extent2D{1024, 1024});
    pPavingStonesTexture->setTexels("../model/PavingStones135_1K.png");

    //! Concrete texture
    auto pConcreteTexture = std::make_unique<Texture>(vk::Extent2D{1024, 1024});
    pConcreteTexture->setTexels("../model/Concrete042A_1K.png");

    //! Wood texture
    auto pWoodTexture = std::make_unique<Texture>(vk::Extent2D{1024, 1024});
    pWoodTexture->setTexels("../model/Wood054_1K.png");

    //! Plane
    auto *pPlaneEntity = getEntityManager().addEntity(std::make_unique<Entity>("plane"));
    pPlaneEntity->addComponent<Mesh>(std::make_unique<Model>("../model/floor.obj"), std::move(pPavingStonesTexture));
    pPlaneEntity->addComponent<Transform>(glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{10.0f, 10.0f, 10.0f});

    //! 42 obj
    auto *p42Entity = getEntityManager().addEntity(std::make_unique<Entity>("42"));
    p42Entity->addComponent<Mesh>(std::make_unique<Model>("../model/forty_two.obj"), std::move(pConcreteTexture));
    p42Entity->addComponent<Transform>(glm::vec3{5.0f, 0.0f, 0.0f});

    //! Vase obj
    auto *pVaseEntity = getEntityManager().addEntity(std::make_unique<Entity>("vase"));
    pVaseEntity->addComponent<Mesh>(std::make_unique<Model>("../model/vase.obj"), std::move(pWoodTexture));
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
