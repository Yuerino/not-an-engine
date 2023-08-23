#include "renderer/Buffer.hpp"

namespace nae {

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

Buffer::~Buffer() {
    unmapMemory();
}

void *Buffer::mapMemory(vk::DeviceSize offset, vk::DeviceSize size) {
    if (mappedMemory_ != nullptr) {
        unmapMemory();
    }
    assert(size + offset <= size_ || size == VK_WHOLE_SIZE);
    mappedMemory_ = vkDeviceMemory_.mapMemory(offset, size);
    return mappedMemory_;
}

void Buffer::unmapMemory() {
    if (mappedMemory_ != nullptr) {
        vkDeviceMemory_.unmapMemory();
        mappedMemory_ = nullptr;
    }
}

void Buffer::flushMemory(vk::DeviceSize offset, vk::DeviceSize size) const {
    device_.get().get().flushMappedMemoryRanges({{*vkDeviceMemory_, offset, size}});
}

vk::DescriptorBufferInfo Buffer::getBufferInfo(vk::DeviceSize offset, vk::DeviceSize size) const noexcept {
    assert(size + offset <= size_ || size == VK_WHOLE_SIZE);
    return vk::DescriptorBufferInfo{*vkBuffer_, offset, size};
}

const vk::raii::Buffer &Buffer::get() const noexcept {
    return vkBuffer_;
}

void *Buffer::getMappedMemory() const noexcept {
    return mappedMemory_;
}

std::array<vk::VertexInputBindingDescription, 1> Vertex::getBindingDescriptions() {
    return {vk::VertexInputBindingDescription{0, sizeof(Vertex), vk::VertexInputRate::eVertex}};
}

std::array<vk::VertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() {
    return {vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
            vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)}};
}

} // namespace nae
