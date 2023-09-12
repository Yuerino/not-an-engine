#pragma once

#include "Component.hpp"
#include "Model.hpp"
#include "Texture.hpp"

namespace nae {

class Mesh final : public Component {
public:
    explicit Mesh(std::unique_ptr<Model> pModel = nullptr, std::unique_ptr<Texture> pTexture = nullptr);

    [[nodiscard]] EComponentType getType() const noexcept final { return EComponentType::Mesh; }

    void setModel(std::unique_ptr<Model> pModel) { pModel_ = std::move(pModel); }
    void setTexture(std::unique_ptr<Texture> pTexture) { pTexture_ = std::move(pTexture); }

    [[nodiscard]] Model *getModel() const noexcept { return pModel_.get(); }
    [[nodiscard]] Texture *getTexture() const noexcept { return pTexture_.get(); }

private:
    std::unique_ptr<Model> pModel_{nullptr};
    std::unique_ptr<Texture> pTexture_{nullptr};
};

} // namespace nae
