#include "sample/FreeCamera.hpp"

#include "core/App.hpp"
#include "scene/Transform.hpp"

namespace nae {

FreeCamera::FreeCamera() {
    // initialize camera buffers and descriptor set
    // TODO: refactor this, descriptor in ubo
    const auto &device = App::get().getGraphicContext().getDevice();
    const auto &renderer = App::get().getRenderer();
    const auto &MAX_FRAMES_IN_FLIGHT = Renderer::getMaxFramesInFlight();

    uboBuffers_.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        uboBuffers_.emplace_back(device, sizeof(CameraUniformBuffer), vk::BufferUsageFlagBits::eUniformBuffer);
        uboBuffers_[i].mapMemory();
    }

    DescriptorSetLayout cameraUBODescriptorSetLayout{
            device,
            {{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}}};
    std::vector<vk::DescriptorSetLayout> vkDescriptorSetLayouts(MAX_FRAMES_IN_FLIGHT,
                                                                *cameraUBODescriptorSetLayout.get());

    pDescriptorSets_ = std::make_unique<DescriptorSets>(device, renderer.getDescriptorPool(), vkDescriptorSetLayouts);
    pDescriptorSets_->update(uboBuffers_);
}

void FreeCamera::onUpdate(Time timestep) {
    auto *pCameraControllerEntity = App::get().getActiveScene().getEntityManager().getEntity("cameraController");
    auto *pCameraTransformComponent = pCameraControllerEntity->getComponent(EComponentType::Transform);
    auto *pCameraTransform = static_cast<Transform *>(pCameraTransformComponent);

    calculateViewMatrix(pCameraTransform->getPosition(), pCameraTransform->getRotation());
    calculateProjectionMatrix(); // TODO: move this to resize scope
}

void FreeCamera::bind(const vk::raii::CommandBuffer &commandBuffer) const {
    auto &renderer = App::get().getRenderer();
    uint32_t currentCommandBufferIdx = renderer.getCurrentCommandBufferIdx();
    auto &descriptorSetsToBind = renderer.getDescriptorSetsToBind();

    CameraUniformBuffer cameraResources{.view = getViewMatrix(), .proj = getProjectionMatrix()};

    uboBuffers_[currentCommandBufferIdx].writeToMemory(cameraResources);
    uboBuffers_[currentCommandBufferIdx].flushMemory();

    // Bind camera descriptor set
    // Bind at set 0 for now TODO: make it configurable
    if (descriptorSetsToBind.empty()) {
        descriptorSetsToBind.emplace_back(*(*pDescriptorSets_)[currentCommandBufferIdx]);
    } else {
        descriptorSetsToBind[0] = *(*pDescriptorSets_)[currentCommandBufferIdx];
    }
}

} // namespace nae
