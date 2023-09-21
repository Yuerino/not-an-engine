#pragma once

#include "Component.hpp"
#include "Material.hpp"
#include "Mesh.hpp"

namespace nae {

struct ObjectPushConstant {
    alignas(16) glm::mat4 transform;
};

class Model final : public Component {
public:
    explicit Model(std::unique_ptr<Mesh> pModel = nullptr, std::unique_ptr<Material> pMaterial = nullptr);

    [[nodiscard]] EComponentType getType() const noexcept final { return EComponentType::Model; }

    void draw(const vk::raii::CommandBuffer &vkCommandBuffer) const;

    void setMesh(std::unique_ptr<Mesh> pMesh) { pMesh_ = std::move(pMesh); }
    void setMaterial(std::unique_ptr<Material> pMaterial) { pMaterial_ = std::move(pMaterial); }

    [[nodiscard]] Mesh *getMesh() const noexcept { return pMesh_.get(); }
    [[nodiscard]] Material *getMaterial() const noexcept { return pMaterial_.get(); }

private:
    std::unique_ptr<Mesh> pMesh_{nullptr};
    std::unique_ptr<Material> pMaterial_{nullptr};
};

} // namespace nae
