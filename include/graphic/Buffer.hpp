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
    void mapMemory(const T *pData, size_t count) {
        assert((propertyFlags_ & vk::MemoryPropertyFlagBits::eHostVisible) &&
               (propertyFlags_ & vk::MemoryPropertyFlagBits::eHostCoherent));
        assert(count * sizeof(T) <= size_);

        void *deviceData = vkDeviceMemory_.mapMemory(0, count * sizeof(T));
        memcpy(deviceData, pData, count * sizeof(T));
        vkDeviceMemory_.unmapMemory();
    }

    //
    //    template<typename DataType>
    //    void upload(const std::vector<DataType> &data, size_t stride = 0) const {
    //        assert(propertyFlags_ & vk::MemoryPropertyFlagBits::eHostVisible);
    //
    //        size_t elementSize = stride ? stride : sizeof(DataType);
    //        assert(sizeof(DataType) <= elementSize);
    //
    //        copyToDevice(vkDeviceMemory_, data.data(), data.size(), elementSize);
    //    }
    //
    //    template<typename DataType>
    //    void upload(const Device &device,
    //                const vk::raii::CommandPool &commandPool,
    //                const vk::raii::Queue &queue,
    //                const std::vector<DataType> &data,
    //                size_t stride) const {
    //        assert(usage_ & vk::BufferUsageFlagBits::eTransferDst);
    //        assert(propertyFlags_ & vk::MemoryPropertyFlagBits::eDeviceLocal);
    //
    //        size_t elementSize = stride ? stride : sizeof(DataType);
    //        assert(sizeof(DataType) <= elementSize);
    //
    //        size_t dataSize = data.size() * elementSize;
    //        assert(dataSize <= size_);
    //
    //        Buffer stagingBuffer{device, dataSize, vk::BufferUsageFlagBits::eTransferSrc};
    //        copyToDevice(stagingBuffer.vkDeviceMemory_, data.data(), data.size(), elementSize);
    //
    //        oneTimeSubmit(device, commandPool, queue, [&](const vk::raii::CommandBuffer &commandBuffer) {
    //            commandBuffer.copyBuffer(*stagingBuffer.vkBuffer_, *this->vkBuffer_, vk::BufferCopy{0, 0, dataSize});
    //        });
    //    }

private:
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
