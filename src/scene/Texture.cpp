#include "scene/Texture.hpp"

#include <cassert>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "core/App.hpp"
#include "renderer/Buffer.hpp"

namespace nae {

Texture::Texture(const vk::Extent2D &extent_,
                 vk::ImageUsageFlags usageFlags,
                 vk::FormatFeatureFlags formatFeatureFlags,
                 bool forceStaging)
    : format_{vk::Format::eR8G8B8A8Srgb}, extent_{extent_} {
    // TODO: check if format is supported
    auto &graphicContext = App::get().getGraphicContext();
    vk::FormatProperties formatProperties = graphicContext.getPhysicalDevice().get().getFormatProperties(format_);

    formatFeatureFlags |= vk::FormatFeatureFlagBits::eSampledImage;
    needStaging_ = forceStaging || (not(formatFeatureFlags & formatProperties.linearTilingFeatures));

    vk::ImageTiling imageTiling{};
    vk::ImageLayout initialLayout{};
    vk::MemoryPropertyFlags requirements;

    if (needStaging_) {
        assert(formatFeatureFlags & formatProperties.optimalTilingFeatures);

        imageTiling = vk::ImageTiling::eOptimal;
        initialLayout = vk::ImageLayout::eUndefined;
        requirements = vk::MemoryPropertyFlagBits::eDeviceLocal;
        usageFlags |= vk::ImageUsageFlagBits::eTransferDst;
    } else {
        imageTiling = vk::ImageTiling::eLinear;
        initialLayout = vk::ImageLayout::ePreinitialized;
        requirements = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
    }

    pTextureImage_ = std::make_unique<Image>(graphicContext.getDevice(),
                                             extent_,
                                             format_,
                                             imageTiling,
                                             usageFlags | vk::ImageUsageFlagBits::eSampled,
                                             initialLayout,
                                             requirements);

    const auto &renderer = App::get().getRenderer();
    DescriptorSetLayout samplerDescriptorSetLayout{
            graphicContext.getDevice(),
            {vk::DescriptorSetLayoutBinding{0,
                                            vk::DescriptorType::eCombinedImageSampler,
                                            1,
                                            vk::ShaderStageFlagBits::eFragment}}};
    pDescriptorSets_ = std::make_unique<DescriptorSets>(
            graphicContext.getDevice(),
            renderer.getDescriptorPool(),
            std::vector<vk::DescriptorSetLayout>{*samplerDescriptorSetLayout.get()});
    vk::DescriptorImageInfo imageInfo{*renderer.getVkSampler(),
                                      *pTextureImage_->getImageView(),
                                      vk::ImageLayout::eShaderReadOnlyOptimal};
    vk::WriteDescriptorSet writeDescriptorSet{*pDescriptorSets_->get()[0],
                                              0,
                                              0,
                                              1,
                                              vk::DescriptorType::eCombinedImageSampler,
                                              &imageInfo};
    graphicContext.getDevice().get().updateDescriptorSets(writeDescriptorSet, nullptr);
}

void Texture::setTexels(const std::string &texelPath) {
    const auto &device = App::get().getGraphicContext().getDevice();
    const auto &vkCommandPool = App::get().getRenderer().getVkCommandPool();

    int texWidth = 0;
    int texHeight = 0;
    int texChannels = 0;
    stbi_uc *pixels = stbi_load(texelPath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(texWidth) * texHeight * 4;
    assert(pixels != nullptr && "failed to load texture image!");
    assert(texWidth == extent_.width && texHeight == extent_.height && "texture size mismatch");
    assert(imageSize == pTextureImage_->get().getMemoryRequirements().size && "image size mismatch");

    if (needStaging_) {
        Buffer stagingBuffer{App::get().getGraphicContext().getDevice(),
                             static_cast<vk::DeviceSize>(extent_.width) * extent_.height * 4,
                             vk::BufferUsageFlagBits::eTransferSrc};
        void *bufferMemory = stagingBuffer.mapMemory(0, imageSize);
        memcpy(bufferMemory, pixels, static_cast<size_t>(imageSize));
        stagingBuffer.flushMemory(0, imageSize);
        stagingBuffer.unmapMemory();

        oneTimeSubmit(device,
                      vkCommandPool,
                      device.getGraphicQueue(),
                      [&](const vk::raii::CommandBuffer &commandBuffer) {
                          setImageLayout(commandBuffer,
                                         *pTextureImage_->get(),
                                         pTextureImage_->getFormat(),
                                         vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::eTransferDstOptimal);
                      });

        oneTimeSubmit(device,
                      vkCommandPool,
                      device.getGraphicQueue(),
                      [&](const vk::raii::CommandBuffer &commandBuffer) {
                          vk::BufferImageCopy copyRegion{
                                  0,
                                  0,
                                  0,
                                  vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
                                  vk::Offset3D{0, 0, 0},
                                  vk::Extent3D{extent_, 1}};

                          commandBuffer.copyBufferToImage(*stagingBuffer.get(),
                                                          *pTextureImage_->get(),
                                                          vk::ImageLayout::eTransferDstOptimal,
                                                          copyRegion);
                      });

        oneTimeSubmit(device,
                      vkCommandPool,
                      device.getGraphicQueue(),
                      [&](const vk::raii::CommandBuffer &commandBuffer) {
                          setImageLayout(commandBuffer,
                                         *pTextureImage_->get(),
                                         pTextureImage_->getFormat(),
                                         vk::ImageLayout::eTransferDstOptimal,
                                         vk::ImageLayout::eShaderReadOnlyOptimal);
                      });
    } else {
        void *imageMemory = pTextureImage_->getDeviceMemory().mapMemory(0, imageSize);
        memcpy(imageMemory, pixels, static_cast<size_t>(imageSize));
        pTextureImage_->getDeviceMemory().unmapMemory();

        oneTimeSubmit(device,
                      vkCommandPool,
                      device.getGraphicQueue(),
                      [&](const vk::raii::CommandBuffer &commandBuffer) {
                          setImageLayout(commandBuffer,
                                         *pTextureImage_->get(),
                                         pTextureImage_->getFormat(),
                                         vk::ImageLayout::ePreinitialized,
                                         vk::ImageLayout::eShaderReadOnlyOptimal);
                      });
    }

    stbi_image_free(pixels);
}

} // namespace nae
