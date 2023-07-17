#ifndef NOT_AN_ENGINE_GRAPHIC_SWAPCHAIN_HPP
#define NOT_AN_ENGINE_GRAPHIC_SWAPCHAIN_HPP

#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace nae {
class Graphic;
}

namespace nae::graphic {

class SwapChainData {
    friend class nae::Graphic;

public:
    SwapChainData(const vk::raii::PhysicalDevice &physicalDevice,
                  const vk::raii::SurfaceKHR &surface,
                  const vk::raii::Device &device,
                  const vk::Extent2D &windowExtent,
                  vk::ImageUsageFlags usage,
                  uint32_t graphicsQueueFamilyIndex,
                  uint32_t presentQueueFamilyIndex);
    ~SwapChainData() = default;

    SwapChainData(const SwapChainData &) = delete;
    SwapChainData &operator=(const SwapChainData &) = delete;
    SwapChainData(SwapChainData &&) = delete;
    SwapChainData &operator=(SwapChainData &&) = delete;

private:
    vk::raii::SwapchainKHR vkSwapChain_ = nullptr;
    vk::Format colorFormat_;
    std::vector<vk::Image> images_;
    std::vector<vk::raii::ImageView> imageViews_;
};

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_SWAPCHAIN_HPP
