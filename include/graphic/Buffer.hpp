#ifndef NOT_AN_ENGINE_GRAPHIC_BUFFER_HPP
#define NOT_AN_ENGINE_GRAPHIC_BUFFER_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "graphic/CommandBuffer.hpp"
#include "graphic/Device.hpp"

namespace nae {
class Graphic;
}

namespace nae::graphic {

class TextureData;

class BufferData {
    friend class ::nae::Graphic;
    friend class TextureData;

public:
    BufferData(const vk::raii::PhysicalDevice &physicalDevice,
               const vk::raii::Device &device,
               vk::DeviceSize size,
               vk::BufferUsageFlags usage,
               vk::MemoryPropertyFlags propertyFlags = vk::MemoryPropertyFlagBits::eHostVisible |
                                                       vk::MemoryPropertyFlagBits::eHostCoherent);
    explicit BufferData(std::nullptr_t) noexcept {}

    template<typename DataType>
    void upload(const DataType &data) const {
        assert((propertyFlags_ & vk::MemoryPropertyFlagBits::eHostCoherent) &&
               (propertyFlags_ & vk::MemoryPropertyFlagBits::eHostVisible));
        assert(sizeof(DataType) <= size_);

        void *dataPtr = deviceMemory_.mapMemory(0, sizeof(DataType));
        memcpy(dataPtr, &data, sizeof(DataType));
        deviceMemory_.unmapMemory();
    }

    template<typename DataType>
    void upload(const std::vector<DataType> &data, size_t stride = 0) const {
        assert(propertyFlags_ & vk::MemoryPropertyFlagBits::eHostVisible);

        size_t elementSize = stride ? stride : sizeof(DataType);
        assert(sizeof(DataType) <= elementSize);

        copyToDevice(deviceMemory_, data.data(), data.size(), elementSize);
    }

    template<typename DataType>
    void upload(const vk::raii::PhysicalDevice &physicalDevice,
                const vk::raii::Device &device,
                const vk::raii::CommandPool &commandPool,
                const vk::raii::Queue &queue,
                const std::vector<DataType> &data,
                size_t stride) const {
        assert(usage_ & vk::BufferUsageFlagBits::eTransferDst);
        assert(propertyFlags_ & vk::MemoryPropertyFlagBits::eDeviceLocal);

        size_t elementSize = stride ? stride : sizeof(DataType);
        assert(sizeof(DataType) <= elementSize);

        size_t dataSize = data.size() * elementSize;
        assert(dataSize <= size_);

        BufferData stagingBuffer{physicalDevice, device, dataSize, vk::BufferUsageFlagBits::eTransferSrc};
        copyToDevice(stagingBuffer.deviceMemory_, data.data(), data.size(), elementSize);

        oneTimeSubmit(device, commandPool, queue, [&](const vk::raii::CommandBuffer &commandBuffer) {
            commandBuffer.copyBuffer(*stagingBuffer.buffer_, *this->buffer_, vk::BufferCopy{0, 0, dataSize});
        });
    }

private:
    vk::raii::Buffer buffer_{nullptr};
    vk::raii::DeviceMemory deviceMemory_{nullptr};

#if !defined(NDEBUG)
    vk::DeviceSize size_{};
    vk::BufferUsageFlags usage_{};
    vk::MemoryPropertyFlags propertyFlags_{};
#endif
};

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_BUFFER_HPP
