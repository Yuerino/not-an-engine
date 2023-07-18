#include "graphic/SwapChain.hpp"

#include <limits>

#include "util.hpp"

namespace nae::graphic {

namespace {

    [[nodiscard]] vk::PresentModeKHR pickPresentMode(std::vector<vk::PresentModeKHR> const &presentModes) {
        vk::PresentModeKHR pickedMode = vk::PresentModeKHR::eFifo;
        for (const auto &presentMode: presentModes) {
            if (presentMode == vk::PresentModeKHR::eMailbox) {
                pickedMode = presentMode;
                break;
            }

            if (presentMode == vk::PresentModeKHR::eImmediate) {
                pickedMode = presentMode;
            }
        }
        return pickedMode;
    }

} // namespace

SwapChainData::SwapChainData(const PhysicalDevice &physicalDevice,
                             const Surface &surface,
                             const vk::raii::Device &device,
                             const vk::Extent2D &windowExtent,
                             vk::ImageUsageFlags usage,
                             uint32_t graphicsQueueFamilyIndex,
                             uint32_t presentQueueFamilyIndex) {
    vk::SurfaceFormatKHR surfaceFormat = surface.pickSurfaceFormat(physicalDevice);
    colorFormat_ = surfaceFormat.format;

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.get().getSurfaceCapabilitiesKHR(*surface.get());
    vk::Extent2D swapChainExtent;

    if (surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
        // If the surface size is undefined, the size is set to the size of the images requested.
        swapChainExtent.width = util::clamp(windowExtent.width,
                                            surfaceCapabilities.minImageExtent.width,
                                            surfaceCapabilities.maxImageExtent.width);
        swapChainExtent.height = util::clamp(windowExtent.height,
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

    vk::PresentModeKHR presentMode = pickPresentMode(physicalDevice.get().getSurfacePresentModesKHR(*surface.get()));
    vk::SwapchainCreateInfoKHR swapChainCreateInfo(vk::SwapchainCreateFlagsKHR{},
                                                   *surface.get(),
                                                   surfaceCapabilities.minImageCount,
                                                   colorFormat_,
                                                   surfaceFormat.colorSpace,
                                                   swapChainExtent,
                                                   1,
                                                   usage,
                                                   vk::SharingMode::eExclusive,
                                                   {},
                                                   preTransform,
                                                   compositeAlpha,
                                                   presentMode,
                                                   true);

    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
        uint32_t queueFamilyIndices[2] = {graphicsQueueFamilyIndex, presentQueueFamilyIndex};
        // If the graphics and present queues are from different queue families, we either have to explicitly
        // transfer ownership of images between the queues, or we have to create the swapchain with imageSharingMode
        // as vk::SharingMode::eConcurrent
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    vkSwapChain_ = vk::raii::SwapchainKHR(device, swapChainCreateInfo);

    images_ = vkSwapChain_.getImages();

    imageViews_.reserve(images_.size());
    vk::ImageViewCreateInfo imageViewCreateInfo(vk::ImageViewCreateFlags{},
                                                {},
                                                vk::ImageViewType::e2D,
                                                colorFormat_,
                                                {},
                                                {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

    for (auto image: images_) {
        imageViewCreateInfo.image = image;
        imageViews_.emplace_back(device, imageViewCreateInfo);
    }
}

} // namespace nae::graphic
