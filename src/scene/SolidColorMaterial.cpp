#include "scene/SolidColorMaterial.hpp"

#include <cassert>

namespace nae {

void SolidColorMaterial::bind(const vk::raii::CommandBuffer &vkCommandBuffer) const {
    assert(getPipeline() != nullptr && "Pipeline is not set");

    vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *getPipeline()->get());

    // TODO: shader reflect for descriptor layout in pipeline first
    // TODO: update and bind descriptor set for color
}

} // namespace nae
