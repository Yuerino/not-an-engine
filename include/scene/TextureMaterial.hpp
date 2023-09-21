#pragma once

#include "Material.hpp"
#include "Texture.hpp"

namespace nae {

class TextureMaterial final : public Material {
public:
    TextureMaterial(std::string name, const std::string &texturePath, vk::Extent2D textureExtent = {1024, 1024});

    void bind(const vk::raii::CommandBuffer &vkCommandBuffer) const final;

    void setTexture(std::unique_ptr<Texture> texture) { pTexture_ = std::move(texture); }
    [[nodiscard]] Texture *getTexture() const noexcept { return pTexture_.get(); }

private:
    std::unique_ptr<Texture> pTexture_;
};

} // namespace nae
