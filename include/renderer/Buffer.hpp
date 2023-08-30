#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "CommandBuffer.hpp"
#include "core/Device.hpp"

namespace nae {

class Buffer {
public:
    Buffer(const Device &device,
           vk::DeviceSize size,
           vk::BufferUsageFlags usage,
           vk::MemoryPropertyFlags propertyFlags = vk::MemoryPropertyFlagBits::eHostVisible);
    ~Buffer();

    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;
    Buffer(Buffer &&) = default;
    Buffer &operator=(Buffer &&) = default;

    void *mapMemory(vk::DeviceSize offset = 0, vk::DeviceSize size = VK_WHOLE_SIZE);

    void unmapMemory();

    void flushMemory(vk::DeviceSize offset = 0, vk::DeviceSize size = VK_WHOLE_SIZE) const;

    [[nodiscard]] vk::DescriptorBufferInfo getBufferInfo(vk::DeviceSize offset = 0,
                                                         vk::DeviceSize size = VK_WHOLE_SIZE) const noexcept;

    [[nodiscard]] const vk::raii::Buffer &get() const noexcept;
    [[nodiscard]] void *getMappedMemory() const noexcept;

    template<typename T>
    void writeToMemory(const T &data, size_t stride = 0) const {
        assert(mappedMemory_ != nullptr && (propertyFlags_ & vk::MemoryPropertyFlagBits::eHostVisible));

        size_t elementSize = stride > 0 ? stride : sizeof(T);
        assert(elementSize <= size_);

        // TODO: Need to assert the mapped memory size is large enough to hold the data
        memcpy(mappedMemory_, &data, elementSize);
    }

    template<typename T>
    void writeToMemory(const std::vector<T> &data, size_t stride = 0) const {
        assert(mappedMemory_ != nullptr && (propertyFlags_ & vk::MemoryPropertyFlagBits::eHostVisible));

        size_t elementSize = stride > 0 ? stride : sizeof(T);
        assert(elementSize <= size_);

        size_t dataSize = data.size() * elementSize;
        assert(dataSize <= size_);

        // TODO: Need to assert the mapped memory size is large enough to hold the data
        memcpy(mappedMemory_, &data, dataSize);
    }

    template<typename T>
    void copyToMemory(const std::vector<T> &data, size_t stride = 0) {
        assert((propertyFlags_ & vk::MemoryPropertyFlagBits::eHostVisible));

        size_t elementSize = stride > 0 ? stride : sizeof(T);
        assert(elementSize <= size_);

        size_t dataSize = data.size() * elementSize;
        assert(dataSize <= size_);

        mapMemory(0, dataSize);
        memcpy(mappedMemory_, data.data(), dataSize);
        flushMemory(0, dataSize);
        unmapMemory();
    }

    template<typename T>
    void copyToDeviceMemory(const vk::raii::CommandPool &commandPool,
                            const vk::raii::Queue &queue,
                            const std::vector<T> &data,
                            size_t stride = 0) const {
        assert((propertyFlags_ & vk::MemoryPropertyFlagBits::eDeviceLocal) &&
               (usage_ & vk::BufferUsageFlagBits::eTransferDst));

        size_t elementSize = stride > 0 ? stride : sizeof(T);
        assert(elementSize <= size_);

        size_t dataSize = data.size() * elementSize;
        assert(dataSize <= size_);

        Buffer stagingBuffer{device_, dataSize, vk::BufferUsageFlagBits::eTransferSrc};
        stagingBuffer.copyToMemory(data, stride);

        oneTimeSubmit(device_, commandPool, queue, [&](const vk::raii::CommandBuffer &commandBuffer) {
            commandBuffer.copyBuffer(*stagingBuffer.vkBuffer_, *this->vkBuffer_, vk::BufferCopy{0, 0, size_});
        });
    }

private:
    std::reference_wrapper<const Device> device_;

    vk::raii::Buffer vkBuffer_{nullptr};
    vk::raii::DeviceMemory vkDeviceMemory_{nullptr};
    void *mappedMemory_{nullptr};

    vk::DeviceSize size_;
    vk::BufferUsageFlags usage_;
    vk::MemoryPropertyFlags propertyFlags_;
};

struct MvpMatrices {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct PushConstantModel {
    alignas(16) glm::mat4 model;
};

} // namespace nae
