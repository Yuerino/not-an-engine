#ifndef NOT_AN_ENGINE_GRAPHIC_TEXTUREDATA_HPP
#define NOT_AN_ENGINE_GRAPHIC_TEXTUREDATA_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "Image.hpp"
#include "core/Device.hpp"

namespace nae::graphic {

class TextureData {
public:
    TextureData(const vk::raii::PhysicalDevice &physicalDevice,
                const Device &device,
                const vk::Extent2D &extent_ = {256, 256},
                vk::ImageUsageFlags usageFlags = {},
                vk::FormatFeatureFlags formatFeatureFlags = {},
                bool anisotropyEnable = false,
                bool forceStaging = false);

    template<typename ImageGenerator>
    void setImage(const vk::raii::CommandBuffer &commandBuffer, const ImageGenerator &imageGenerator) {
        void *data = needStaging_
                             ? stagingBufferData_.deviceMemory_.copyToMemory(
                                       0,
                                       stagingBufferData_.buffer_.getMemoryRequirements().size)
                             : imageData_.deviceMemory_.mapMemory(0, imageData_.image_.getMemoryRequirements().size);
        imageGenerator(data, extent_);
        needStaging_ ? stagingBufferData_.deviceMemory_.unmapMemory() : imageData_.deviceMemory_.unmapMemory();

        if (needStaging_) {
            // Since we're going to blit to the texture image, set its layout to eTransferDstOptimal
            setImageLayout(commandBuffer,
                           *imageData_.image_,
                           imageData_.format_,
                           vk::ImageLayout::eUndefined,
                           vk::ImageLayout::eTransferDstOptimal);
            vk::BufferImageCopy copyRegion{0,
                                           extent_.width,
                                           extent_.height,
                                           vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
                                           vk::Offset3D{0, 0, 0},
                                           vk::Extent3D{extent_, 1}};
            commandBuffer.copyBufferToImage(*stagingBufferData_.buffer_,
                                            *imageData_.image_,
                                            vk::ImageLayout::eTransferDstOptimal,
                                            copyRegion);
            // Set the layout for the texture image from eTransferDstOptimal to eShaderReadOnlyOptimal
            setImageLayout(commandBuffer,
                           *imageData_.image_,
                           imageData_.format_,
                           vk::ImageLayout::eTransferDstOptimal,
                           vk::ImageLayout::eShaderReadOnlyOptimal);
        } else {
            // If we can use the linear tiled image as a texture, just do it
            setImageLayout(commandBuffer,
                           *imageData_.image_,
                           imageData_.format_,
                           vk::ImageLayout::ePreinitialized,
                           vk::ImageLayout::eShaderReadOnlyOptimal);
        }
    }

    friend void updateDescriptorSets(const vk::raii::Device &device,
                                     const vk::raii::DescriptorSet &descriptorSet,
                                     const std::vector<std::tuple<vk::DescriptorType,
                                                                  const vk::raii::Buffer &,
                                                                  vk::DeviceSize,
                                                                  const vk::raii::BufferView *>> &bufferData,
                                     const std::vector<TextureData> &textureData,
                                     uint32_t bindingOffset);

private:
    vk::Format format_;
    vk::Extent2D extent_;
    bool needStaging_;
    BufferData stagingBufferData_{nullptr};
    Image imageData_{nullptr};
    vk::raii::Sampler vkSampler_;
};

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_TEXTUREDATA_HPP
