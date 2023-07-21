#include "graphic/SwapChain.hpp"

#include <limits>

namespace nae::graphic {

SwapChain::SwapChain(const PhysicalDevice &physicalDevice,
                     const Surface &surface,
                     const Device &device,
                     vk::ImageUsageFlags usage) {
    surfaceFormat_ = surface.pickSurfaceFormat(physicalDevice);
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.get().getSurfaceCapabilitiesKHR(*surface.get());
    vk::Extent2D swapChainExtent;

    if (surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
        // If the surface size is undefined, the size is set to the size of the images requested.
        auto windowExtent = surface.getWindow().getExtent();
        swapChainExtent.width = std::clamp(windowExtent.width,
                                           surfaceCapabilities.minImageExtent.width,
                                           surfaceCapabilities.maxImageExtent.width);
        swapChainExtent.height = std::clamp(windowExtent.height,
                                            surfaceCapabilities.minImageExtent.height,
                                            surfaceCapabilities.maxImageExtent.height);
    } else {
        swapChainExtent = surfaceCapabilities.currentExtent;
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
                                                   surfaceFormat_.format,
                                                   surfaceFormat_.colorSpace,
                                                   swapChainExtent,
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
        uint32_t queueFamilyIndices[2] = {device.getGraphicQueueFamilyIndex(), device.getPresentQueueFamilyIndex()};
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    vkSwapChain_ = vk::raii::SwapchainKHR{device.get(), swapChainCreateInfo};

    images_ = vkSwapChain_.getImages();

    imageViews_.reserve(images_.size());
    vk::ImageViewCreateInfo imageViewCreateInfo{vk::ImageViewCreateFlags{},
                                                {},
                                                vk::ImageViewType::e2D,
                                                surfaceFormat_.format,
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

const vk::raii::SwapchainKHR &SwapChain::get() const noexcept {
    return vkSwapChain_;
}

const std::vector<vk::Image> &SwapChain::getImages() const noexcept {
    return images_;
}

const std::vector<vk::raii::ImageView> &SwapChain::getImageViews() const noexcept {
    return imageViews_;
}

} // namespace nae::graphic
