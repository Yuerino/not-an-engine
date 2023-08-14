#include "graphic/Buffer.hpp"

namespace nae::graphic {

Buffer::Buffer(const Device &device,
               vk::DeviceSize size,
               vk::BufferUsageFlags usage,
               vk::MemoryPropertyFlags propertyFlags)
    : device_{device},
      vkBuffer_{device.get(), {vk::BufferCreateFlags{}, size, usage, vk::SharingMode::eExclusive}},
      vkDeviceMemory_{device.createDeviceMemory(vkBuffer_.getMemoryRequirements(), propertyFlags)},
      size_{size},
      usage_{usage},
      propertyFlags_{propertyFlags} {
    vkBuffer_.bindMemory(*vkDeviceMemory_, 0);
}

const vk::raii::Buffer &Buffer::get() const noexcept {
    return vkBuffer_;
}

std::array<vk::VertexInputBindingDescription, 1> Vertex::getBindingDescriptions() {
    return {vk::VertexInputBindingDescription{0, sizeof(Vertex), vk::VertexInputRate::eVertex}};
}

std::array<vk::VertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() {
    return {vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)},
            vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)}};
}

} // namespace nae::graphic
