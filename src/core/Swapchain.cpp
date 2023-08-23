#include "core/Swapchain.hpp"

#include <array>
#include <limits>

namespace nae {

Swapchain::Swapchain(const PhysicalDevice &physicalDevice,
                     const Device &device,
                     const Surface &surface,
                     vk::Extent2D extent,
                     vk::ImageUsageFlags usage) {
    format_ = surface.pickSurfaceFormat(physicalDevice);
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.get().getSurfaceCapabilitiesKHR(*surface.get());

    if (surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
        // If the surface size is undefined, the size is set to the size of the images requested.
        extent_.width = std::clamp(extent.width,
                                   surfaceCapabilities.minImageExtent.width,
                                   surfaceCapabilities.maxImageExtent.width);
        extent_.height = std::clamp(extent.height,
                                    surfaceCapabilities.minImageExtent.height,
                                    surfaceCapabilities.maxImageExtent.height);
    } else {
        extent_ = surfaceCapabilities.currentExtent;
    }

    vk::SurfaceTransformFlagBitsKHR preTransform = (surfaceCapabilities.supportedTransforms &
                                                    vk::SurfaceTransformFlagBitsKHR::eIdentity)
                                                           ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                                                           : surfaceCapabilities.currentTransform;

    vk::CompositeAlphaFlagBitsKHR compositeAlpha =
            (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
                    ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
            : (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
                    ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
            : (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)
                    ? vk::CompositeAlphaFlagBitsKHR::eInherit
                    : vk::CompositeAlphaFlagBitsKHR::eOpaque;

    vk::PresentModeKHR presentMode = surface.pickPresentMode(physicalDevice);

    uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{vk::SwapchainCreateFlagsKHR{},
                                                   *surface.get(),
                                                   minImageCount,
                                                   format_.format,
                                                   format_.colorSpace,
                                                   extent_,
                                                   1,
                                                   usage,
                                                   vk::SharingMode::eExclusive,
                                                   {},
                                                   preTransform,
                                                   compositeAlpha,
                                                   presentMode,
                                                   true};

    // If the graphics and present queues are from different queue families, we either have to explicitly
    // transfer ownership of images between the queues, or we have to create the swapchain with imageSharingMode
    // as vk::SharingMode::eConcurrent
    if (device.getGraphicQueueFamilyIndex() != device.getPresentQueueFamilyIndex()) {
        std::array<uint32_t, 2> queueFamilyIndices = {device.getGraphicQueueFamilyIndex(),
                                                      device.getPresentQueueFamilyIndex()};
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }

    vkSwapChain_ = vk::raii::SwapchainKHR{device.get(), swapChainCreateInfo};

    images_ = vkSwapChain_.getImages();

    imageViews_.reserve(images_.size());
    vk::ImageViewCreateInfo imageViewCreateInfo{vk::ImageViewCreateFlags{},
                                                {},
                                                vk::ImageViewType::e2D,
                                                format_.format,
                                                {vk::ComponentSwizzle::eIdentity,
                                                 vk::ComponentSwizzle::eIdentity,
                                                 vk::ComponentSwizzle::eIdentity,
                                                 vk::ComponentSwizzle::eIdentity},
                                                {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};

    for (auto image: images_) {
        imageViewCreateInfo.image = image;
        imageViews_.emplace_back(device.get(), imageViewCreateInfo);
    }
}

void Swapchain::createDepthBuffer(const Device &device) {
    // TODO: query depth format
    pDepthBuffer_ = std::make_unique<DepthImage>(device, extent_, vk::Format::eD32Sfloat);
}

void Swapchain::createFrameBuffers(const Device &device, const RenderPass &renderPass) {
    std::array<vk::ImageView, 2> attachments;
    attachments[1] = pDepthBuffer_ ? *pDepthBuffer_->getImageView() : vk::ImageView{};

    vk::FramebufferCreateInfo framebufferCreateInfo{vk::FramebufferCreateFlags{},
                                                    *renderPass.get(),
                                                    static_cast<uint32_t>(pDepthBuffer_ ? 2 : 1),
                                                    attachments.data(),
                                                    extent_.width,
                                                    extent_.height,
                                                    1};

    frameBuffers_.clear();
    frameBuffers_.reserve(imageViews_.size());
    for (const auto &imageView: imageViews_) {
        attachments[0] = *imageView;
        frameBuffers_.emplace_back(device.get(), framebufferCreateInfo);
    }
}

const vk::raii::SwapchainKHR &Swapchain::get() const noexcept {
    return vkSwapChain_;
}

const vk::SurfaceFormatKHR &Swapchain::getFormat() const noexcept {
    return format_;
}

const vk::Extent2D &Swapchain::getExtent() const noexcept {
    return extent_;
}

const std::vector<vk::Image> &Swapchain::getImages() const noexcept {
    return images_;
}

const std::vector<vk::raii::ImageView> &Swapchain::getImageViews() const noexcept {
    return imageViews_;
}

const std::vector<vk::raii::Framebuffer> &Swapchain::getFrameBuffers() const noexcept {
    return frameBuffers_;
}

} // namespace nae
