#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "core/Device.hpp"
#include "core/PhysicalDevice.hpp"
#include "core/Surface.hpp"
#include "renderer/Image.hpp"
#include "renderer/RenderPass.hpp"

namespace nae {

class Device;
class PhysicalDevice;
class Surface;
class DepthImage;
class RenderPass;

class Swapchain {
public:
    Swapchain(const PhysicalDevice &physicalDevice,
              const Device &device,
              const Surface &surface,
              vk::Extent2D extent,
              vk::ImageUsageFlags usage);
    ~Swapchain() = default;

    Swapchain(const Swapchain &) = delete;
    Swapchain &operator=(const Swapchain &) = delete;
    Swapchain(Swapchain &&) = default;
    Swapchain &operator=(Swapchain &&) = default;

    void createDepthBuffer(const Device &device);

    void createFrameBuffers(const Device &device, const RenderPass &renderPass);

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
    std::unique_ptr<DepthImage> pDepthBuffer_;
};

} // namespace nae
