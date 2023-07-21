#include "graphic/Surface.hpp"

#include <cassert>

namespace nae::graphic {

Surface::Surface(const Instance &instance, const Window &window) : window_{window} {
    VkSurfaceKHR surface;
    window_.createVulkanSurface(static_cast<VkInstance>(*instance.get()), nullptr, &surface);
    vkSurface_ = vk::raii::SurfaceKHR{instance.get(), surface};
}

vk::SurfaceFormatKHR Surface::pickSurfaceFormat(const PhysicalDevice &physicalDevice) const {
    auto formats = physicalDevice.get().getSurfaceFormatsKHR(*vkSurface_);
    assert(!formats.empty());
    vk::SurfaceFormatKHR pickedFormat = formats[0];

    if (formats.size() == 1) {
        if (formats[0].format == vk::Format::eUndefined) {
            pickedFormat.format = vk::Format::eB8G8R8A8Srgb;
            pickedFormat.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        }
    } else {
        // Select first one found
        vk::Format requestedFormats[] = {vk::Format::eB8G8R8A8Srgb,
                                         vk::Format::eR8G8B8A8Srgb,
                                         vk::Format::eB8G8R8A8Unorm,
                                         vk::Format::eR8G8B8A8Unorm};
        vk::ColorSpaceKHR requestedColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

        for (const auto &requestedFormat: requestedFormats) {
            auto it = std::find_if(formats.begin(),
                                   formats.end(),
                                   [&requestedFormat, &requestedColorSpace](const vk::SurfaceFormatKHR &f) {
                                       return (f.format == requestedFormat) && (f.colorSpace == requestedColorSpace);
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

vk::PresentModeKHR Surface::pickPresentMode(const PhysicalDevice &physicalDevice) const {
    auto presentModes = physicalDevice.get().getSurfacePresentModesKHR(*vkSurface_);
    assert(!presentModes.empty());

    vk::PresentModeKHR pickedMode = vk::PresentModeKHR::eFifo;

    for (const auto &presentMode: presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            pickedMode = presentMode;
            break;
        } else if (presentMode == vk::PresentModeKHR::eImmediate) {
            pickedMode = presentMode;
        }
    }

    return pickedMode;
}

const vk::raii::SurfaceKHR &Surface::get() const noexcept {
    return vkSurface_;
}

const ::nae::Window &Surface::getWindow() const noexcept {
    return window_;
}

} // namespace nae::graphic
