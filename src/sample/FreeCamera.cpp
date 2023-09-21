#include "sample/FreeCamera.hpp"

#include "core/App.hpp"
#include "scene/Transform.hpp"

namespace nae {

void FreeCamera::onUpdate(Time timestep) {
    auto *pCameraControllerEntity = App::get().getActiveScene().getEntityManager().getEntity("cameraController");
    auto *pCameraTransformComponent = pCameraControllerEntity->getComponent(EComponentType::Transform);
    auto *pCameraTransform = static_cast<Transform *>(pCameraTransformComponent);

    calculateViewMatrix(pCameraTransform->getPosition(), pCameraTransform->getRotation());
    calculateProjectionMatrix(); // TODO: move this to resize scope
}

} // namespace nae
