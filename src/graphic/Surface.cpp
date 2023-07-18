#include "graphic/Surface.hpp"

#include <cassert>
#include <vulkan/vulkan.hpp>

#include "GlfwApi.hpp"

namespace nae::graphic {

vk::raii::SurfaceKHR createSurface(const vk::raii::Instance &vkInstance, GLFWwindow *glfwWindow) {
    VkSurfaceKHR surface;

    auto result = glfwWrapper([&]() {
        return glfwCreateWindowSurface(static_cast<VkInstance>(*vkInstance), glfwWindow, nullptr, &surface);
    });
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }

    return vk::raii::SurfaceKHR{vkInstance, surface};
}

vk::SurfaceFormatKHR pickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &formats) {
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

} // namespace nae::graphic
