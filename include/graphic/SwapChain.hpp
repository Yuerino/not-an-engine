#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "graphic/Device.hpp"
#include "graphic/PhysicalDevice.hpp"
#include "graphic/RenderPass.hpp"
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

    void createFrameBuffers(const Device &device,
                            const RenderPass &renderPass,
                            const vk::raii::ImageView *depthImageViewPtr = nullptr);

    [[nodiscard]] const vk::raii::SwapchainKHR &get() const noexcept;
    [[nodiscard]] const vk::SurfaceFormatKHR &getFormat() const noexcept;
    [[nodiscard]] const vk::Extent2D &getExtent() const noexcept;
    [[nodiscard]] const std::vector<vk::Image> &getImages() const noexcept;
    [[nodiscard]] const std::vector<vk::raii::ImageView> &getImageViews() const noexcept;
    [[nodiscard]] const std::vector<vk::raii::Framebuffer> &getFrameBuffers() const noexcept;

private:
    vk::raii::SwapchainKHR vkSwapChain_{nullptr};
    vk::SurfaceFormatKHR format_;
    vk::Extent2D extent_;
    std::vector<vk::Image> images_;
    std::vector<vk::raii::ImageView> imageViews_;
    std::vector<vk::raii::Framebuffer> frameBuffers_;
};

} // namespace nae::graphic
