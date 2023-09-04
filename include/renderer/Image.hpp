#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "core/Device.hpp"

namespace nae {

class Device;

class Image {
public:
    Image(const Device &device,
          const vk::Extent2D &extent,
          vk::Format format,
          vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
          vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled,
          vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined,
          vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
          vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor);

    [[nodiscard]] const vk::raii::Image &get() const noexcept;
    [[nodiscard]] const vk::raii::DeviceMemory &getDeviceMemory() const noexcept { return deviceMemory_; }
    [[nodiscard]] const vk::raii::ImageView &getImageView() const noexcept;
    [[nodiscard]] vk::Format getFormat() const noexcept { return format_; }

private:
    vk::Format format_{};
    vk::raii::Image image_{nullptr};
    vk::raii::DeviceMemory deviceMemory_{nullptr};
    vk::raii::ImageView imageView_{nullptr};
};

class DepthImage : public Image {
public:
    DepthImage(const Device &device, const vk::Extent2D &extent, vk::Format format)
        : Image(device,
                extent,
                format,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::ImageLayout::eUndefined,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                vk::ImageAspectFlagBits::eDepth) {}
};

} // namespace nae
