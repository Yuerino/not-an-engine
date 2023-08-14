#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "graphic/CommandBuffer.hpp"
#include "graphic/Device.hpp"

namespace nae::graphic {

class Buffer {
public:
    Buffer(const Device &device,
           vk::DeviceSize size,
           vk::BufferUsageFlags usage,
           vk::MemoryPropertyFlags propertyFlags = vk::MemoryPropertyFlagBits::eHostVisible |
                                                   vk::MemoryPropertyFlagBits::eHostCoherent);
    ~Buffer() = default;

    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;
    Buffer(Buffer &&) = default;
    Buffer &operator=(Buffer &&) = default;

    [[nodiscard]] const vk::raii::Buffer &get() const noexcept;

    template<typename T>
    void mapMemory(const std::vector<T> &data, size_t stride = 0) {
        assert((propertyFlags_ & vk::MemoryPropertyFlagBits::eHostVisible) &&
               (propertyFlags_ & vk::MemoryPropertyFlagBits::eHostCoherent));

        size_t elementSize = stride ? stride : sizeof(T);
        assert(elementSize <= size_);

        size_t dataSize = data.size() * elementSize;
        assert(dataSize <= size_);

        void *deviceData = vkDeviceMemory_.mapMemory(0, dataSize);
        memcpy(deviceData, data.data(), dataSize);
        vkDeviceMemory_.unmapMemory();
    }

    template<typename T>
    void mapLocalMemory(const vk::raii::CommandPool &commandPool,
                        const vk::raii::Queue &queue,
                        const std::vector<T> &data,
                        size_t stride = 0) {
        assert((propertyFlags_ & vk::MemoryPropertyFlagBits::eDeviceLocal) &&
               (usage_ & vk::BufferUsageFlagBits::eTransferDst));

        size_t elementSize = stride ? stride : sizeof(T);
        assert(elementSize <= size_);

        size_t dataSize = data.size() * elementSize;
        assert(dataSize <= size_);

        Buffer stagingBuffer{device_, dataSize, vk::BufferUsageFlagBits::eTransferSrc};
        stagingBuffer.mapMemory(data, stride);

        oneTimeSubmit(device_, commandPool, queue, [&](const vk::raii::CommandBuffer &commandBuffer) {
            commandBuffer.copyBuffer(*stagingBuffer.vkBuffer_, *this->vkBuffer_, vk::BufferCopy{0, 0, size_});
        });
    }

private:
    std::reference_wrapper<const Device> device_;

    vk::raii::Buffer vkBuffer_{nullptr};
    vk::raii::DeviceMemory vkDeviceMemory_{nullptr};

    vk::DeviceSize size_;
    vk::BufferUsageFlags usage_;
    vk::MemoryPropertyFlags propertyFlags_;
};

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    [[nodiscard]] static std::array<vk::VertexInputBindingDescription, 1> getBindingDescriptions();
    [[nodiscard]] static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions();
};

} // namespace nae::graphic
