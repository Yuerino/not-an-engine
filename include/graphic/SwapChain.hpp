#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "graphic/Device.hpp"
#include "graphic/PhysicalDevice.hpp"
#include "graphic/Surface.hpp"

namespace nae::graphic {

class SwapChain {
public:
    SwapChain(const PhysicalDevice &physicalDevice,
              const Surface &surface,
              const Device &device,
              vk::ImageUsageFlags usage);

    SwapChain(const SwapChain &) = delete;
    SwapChain &operator=(const SwapChain &) = delete;

    [[nodiscard]] const vk::raii::SwapchainKHR &get() const noexcept;
    [[nodiscard]] const std::vector<vk::Image> &getImages() const noexcept;
    [[nodiscard]] const std::vector<vk::raii::ImageView> &getImageViews() const noexcept;

private:
    vk::raii::SwapchainKHR vkSwapChain_{nullptr};
    vk::SurfaceFormatKHR surfaceFormat_;
    std::vector<vk::Image> images_;
    std::vector<vk::raii::ImageView> imageViews_;
};

} // namespace nae::graphic
