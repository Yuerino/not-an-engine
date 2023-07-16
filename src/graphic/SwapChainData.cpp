#include "graphic/SwapChainData.hpp"

#include <cassert>
#include <limits>

#include "util.hpp"

namespace nae::graphic {

namespace {

    [[nodiscard]] vk::SurfaceFormatKHR pickSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &formats) {
        assert(!formats.empty());
        vk::SurfaceFormatKHR pickedFormat = formats[0];

        if (formats.size() == 1) {
            if (formats[0].format == vk::Format::eUndefined) {
                pickedFormat.format = vk::Format::eB8G8R8A8Unorm;
                pickedFormat.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
            }
        } else {
            // Select first one found
            vk::Format requestedFormats[] = {vk::Format::eB8G8R8A8Unorm,
                                             vk::Format::eR8G8B8A8Unorm,
                                             vk::Format::eB8G8R8Unorm,
                                             vk::Format::eR8G8B8Unorm};
            vk::ColorSpaceKHR requestedColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

            for (const auto &requestedFormat: requestedFormats) {
                auto it = std::find_if(formats.begin(),
                                       formats.end(),
                                       [&requestedFormat, &requestedColorSpace](vk::SurfaceFormatKHR const &f) {
                                           return (f.format == requestedFormat) &&
                                                  (f.colorSpace == requestedColorSpace);
                                       });
                if (it != formats.end()) {
                    pickedFormat = *it;
                    break;
                }
            }
        }

        assert(pickedFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
        return pickedFormat;
    }

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

SwapChainData::SwapChainData(const vk::raii::PhysicalDevice &physicalDevice,
                             const vk::raii::SurfaceKHR &surface,
                             const vk::raii::Device &device,
                             const vk::Extent2D &windowExtent,
                             vk::ImageUsageFlags usage,
                             uint32_t graphicsQueueFamilyIndex,
                             uint32_t presentQueueFamilyIndex) {
    vk::SurfaceFormatKHR surfaceFormat = pickSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(*surface));
    colorFormat_ = surfaceFormat.format;

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
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

    vk::PresentModeKHR presentMode = pickPresentMode(physicalDevice.getSurfacePresentModesKHR(*surface));
    vk::SwapchainCreateInfoKHR swapChainCreateInfo(vk::SwapchainCreateFlagsKHR{},
                                                   *surface,
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
