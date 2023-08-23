#include "renderer/Texture.hpp"

#include <cassert>

namespace nae::graphic {

TextureData::TextureData(const vk::raii::PhysicalDevice &physicalDevice,
                         const Device &device,
                         const vk::Extent2D &extent_,
                         vk::ImageUsageFlags usageFlags,
                         vk::FormatFeatureFlags formatFeatureFlags,
                         bool anisotropyEnable,
                         bool forceStaging)
    : format_{vk::Format::eR8G8B8A8Unorm},
      extent_{extent_},
      vkSampler_{device.get(),
                 {vk::SamplerCreateFlags{},
                  vk::Filter::eLinear,
                  vk::Filter::eLinear,
                  vk::SamplerMipmapMode::eLinear,
                  vk::SamplerAddressMode::eRepeat,
                  vk::SamplerAddressMode::eRepeat,
                  vk::SamplerAddressMode::eRepeat,
                  0.0f,
                  anisotropyEnable,
                  16.0f,
                  false,
                  vk::CompareOp::eAlways,
                  0.0f,
                  0.0f,
                  vk::BorderColor::eFloatOpaqueWhite,
                  false}} {
    vk::FormatProperties formatProperties = physicalDevice.getFormatProperties(format_);

    formatFeatureFlags |= vk::FormatFeatureFlagBits::eSampledImage;
    needStaging_ = forceStaging || ((formatProperties.linearTilingFeatures & formatFeatureFlags) != formatFeatureFlags);

    vk::ImageTiling imageTiling;
    vk::ImageLayout initialLayout;
    vk::MemoryPropertyFlags requirements;

    if (needStaging_) {
        assert((formatProperties.optimalTilingFeatures & formatFeatureFlags) == formatFeatureFlags);
        stagingBufferData_ =
                BufferData{device, extent_.width * extent_.height * 4, vk::BufferUsageFlagBits::eTransferSrc};
        imageTiling = vk::ImageTiling::eOptimal;
        usageFlags |= vk::ImageUsageFlagBits::eTransferDst;
        initialLayout = vk::ImageLayout::eUndefined;
    } else {
        imageTiling = vk::ImageTiling::eLinear;
        initialLayout = vk::ImageLayout::ePreinitialized;
        requirements = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
    }

    imageData_ = Image{device,
                       format_,
                           extent_,
                           imageTiling,
                           usageFlags | vk::ImageUsageFlagBits::eSampled,
                           initialLayout,
                           requirements,
                           vk::ImageAspectFlagBits::eColor};
}

} // namespace nae::graphic
