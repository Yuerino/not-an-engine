#include "renderer/Image.hpp"

namespace nae {

Image::Image(const Device &device,
             const vk::Extent2D &extent,
             vk::Format format,
             vk::ImageTiling tiling,
             vk::ImageUsageFlags usage,
             vk::ImageLayout initialLayout,
             vk::MemoryPropertyFlags memoryProperties,
             vk::ImageAspectFlags aspectMask)
    : format_{format},
      image_{device.get(),
             {vk::ImageCreateFlags{},
              vk::ImageType::e2D,
              format,
              vk::Extent3D{extent, 1},
              1,
              1,
              vk::SampleCountFlagBits::e1,
              tiling,
              usage | vk::ImageUsageFlagBits::eSampled,
              vk::SharingMode::eExclusive,
              {},
              initialLayout}},
      deviceMemory_{device.createDeviceMemory(image_.getMemoryRequirements(), memoryProperties)} {
    image_.bindMemory(*deviceMemory_, 0);
    imageView_ = vk::raii::ImageView{device.get(),
                                     {vk::ImageViewCreateFlags{},
                                      *image_,
                                      vk::ImageViewType::e2D,
                                      format,
                                      vk::ComponentMapping{},
                                      {aspectMask, 0, 1, 0, 1}}};
}

const vk::raii::Image &Image::get() const noexcept {
    return image_;
}

const vk::raii::ImageView &Image::getImageView() const noexcept {
    return imageView_;
}

} // namespace nae
