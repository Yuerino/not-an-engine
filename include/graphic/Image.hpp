#ifndef NOT_AN_ENGINE_GRAPHIC_IMAGE_HPP
#define NOT_AN_ENGINE_GRAPHIC_IMAGE_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace nae {
class Graphic;
}

namespace nae::graphic {

class TextureData;

class ImageData {
    friend class ::nae::Graphic;
    friend class TextureData;

public:
    ImageData(const vk::raii::PhysicalDevice &physicalDevice,
              const vk::raii::Device &device,
              vk::Format format,
              const vk::Extent2D &extent,
              vk::ImageTiling tiling,
              vk::ImageUsageFlags usage,
              vk::ImageLayout initialLayout,
              vk::MemoryPropertyFlags memoryProperties,
              vk::ImageAspectFlags aspectMask);
    explicit ImageData(std::nullptr_t) noexcept {}

    friend void updateDescriptorSets(const vk::raii::Device &device,
                                     const vk::raii::DescriptorSet &descriptorSet,
                                     const std::vector<std::tuple<vk::DescriptorType,
                                                                  const vk::raii::Buffer &,
                                                                  vk::DeviceSize,
                                                                  const vk::raii::BufferView *>> &bufferData,
                                     const std::vector<TextureData> &textureData,
                                     uint32_t bindingOffset);

private:
    vk::Format format_{};
    vk::raii::Image image_{nullptr};
    vk::raii::DeviceMemory deviceMemory_{nullptr};
    vk::raii::ImageView imageView_{nullptr};
};

class DepthBufferData : public ImageData {
public:
    DepthBufferData(const vk::raii::PhysicalDevice &physicalDevice,
                    const vk::raii::Device &device,
                    vk::Format format,
                    const vk::Extent2D &extent)
        : ImageData(physicalDevice,
                    device,
                    format,
                    extent,
                    vk::ImageTiling::eOptimal,
                    vk::ImageUsageFlagBits::eDepthStencilAttachment,
                    vk::ImageLayout::eUndefined,
                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                    vk::ImageAspectFlagBits::eDepth) {}
    explicit DepthBufferData(std::nullptr_t) noexcept : ImageData{nullptr} {}
};

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_IMAGE_HPP
