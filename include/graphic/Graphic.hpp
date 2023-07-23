#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "Window.hpp"
#include "graphic/Buffer.hpp"
#include "graphic/Device.hpp"
#include "graphic/Image.hpp"
#include "graphic/Instance.hpp"
#include "graphic/PhysicalDevice.hpp"
#include "graphic/Pipeline.hpp"
#include "graphic/Surface.hpp"
#include "graphic/SwapChain.hpp"

namespace nae {

class Graphic {
public:
    explicit Graphic(Window &window);
    ~Graphic();

    Graphic(const Graphic &) = delete;
    Graphic &operator=(const Graphic &) = delete;

    void Update();

private:
    void recreateSwapChain();

    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame_ = 0;
    Window &window_;

    graphic::Instance instance_;
    graphic::Surface surface_;
    graphic::PhysicalDevice physicalDevice_;
    graphic::Device device_;
    std::unique_ptr<graphic::SwapChain> pSwapChain_;
    graphic::Pipeline pipeline_;

    vk::raii::CommandPool vkCommandPool_{nullptr};
    vk::raii::CommandBuffers vkCommandBuffers_{nullptr};

    std::vector<vk::raii::Semaphore> imageAcquiredSemaphores_;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores_;
    std::vector<vk::raii::Fence> drawFences_;
};

} // namespace nae
