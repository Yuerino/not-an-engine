#include "scene/Model.hpp"

#include <vulkan/vulkan_raii.hpp>

#include "core/App.hpp"
#include "scene/SolidColorMaterial.hpp"
#include "scene/Transform.hpp"

namespace nae {

Model::Model(std::unique_ptr<Mesh> pMesh, std::unique_ptr<Material> pMaterial)
    : pMesh_{std::move(pMesh)}, pMaterial_{std::move(pMaterial)} {
    if (pMesh != nullptr && pMaterial == nullptr) {
        pMaterial_ = std::make_unique<SolidColorMaterial>("white", glm::vec3{1.0f, 1.0f, 1.0f});
    }
}

void Model::draw(const vk::raii::CommandBuffer &vkCommandBuffer) const {
    if (pMesh_ == nullptr || pMaterial_ == nullptr) {
        return;
    }

    // Update object resource
    Component *pComponent = getEntity()->getComponent(EComponentType::Transform);
    auto *pTransform = static_cast<Transform *>(pComponent);
    ObjectPushConstant objectPushConstant{.transform = pTransform->getWorldMatrix()};

    // Bind object resource
    vkCommandBuffer.pushConstants<ObjectPushConstant>(*pMaterial_->getPipeline()->getLayout(),
                                                      vk::ShaderStageFlagBits::eVertex,
                                                      0,
                                                      objectPushConstant);

    // Bind material
    pMaterial_->bind(vkCommandBuffer);

    // Bind descriptor sets
    auto descriptorSetsToBind = App::get().getRenderer().getDescriptorSetsToBind();
    vkCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                       *pMaterial_->getPipeline()->getLayout(),
                                       0,
                                       descriptorSetsToBind,
                                       {});

    // Bind mesh
    pMesh_->draw(vkCommandBuffer);
}

} // namespace nae
