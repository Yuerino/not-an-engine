#include "scene/TextureMaterial.hpp"

#include <cassert>

namespace nae {

TextureMaterial::TextureMaterial(std::string name, const std::string &texturePath, vk::Extent2D textureExtent)
    : Material(std::move(name),
               std::make_unique<Pipeline>("resource/shader/TextureMaterial.vert.spv",
                                          "resource/shader/TextureMaterial.frag.spv")),
      pTexture_{std::make_unique<Texture>(textureExtent)} {
    pTexture_->setTexels(texturePath);
}

void TextureMaterial::bind(const vk::raii::CommandBuffer &vkCommandBuffer) const {
    assert(getPipeline() != nullptr && "Pipeline is not set");
    assert(pTexture_ != nullptr && "Texture is not set");

    vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *getPipeline()->get());

    pTexture_->bind(vkCommandBuffer);
}

} // namespace nae
