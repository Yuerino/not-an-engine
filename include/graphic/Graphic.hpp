#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "Window.hpp"
#include "graphic/Buffer.hpp"
#include "graphic/Descriptor.hpp"
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
    Graphic(Graphic &&) = delete;
    Graphic &operator=(Graphic &&) = delete;

    void Update();

private:
    void recreateSwapChain();
    void updateMvpMatrix(uint32_t currentFrame) const;

    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame_ = 0;

    std::reference_wrapper<Window> window_;

    graphic::Instance instance_;
    graphic::Surface surface_;
    graphic::PhysicalDevice physicalDevice_;
    graphic::Device device_;
    std::unique_ptr<graphic::SwapChain> pSwapChain_;
    graphic::DescriptorPool descriptorPool_;
    graphic::DescriptorSetLayout descriptorSetLayout_;
    std::unique_ptr<graphic::DescriptorSets> pDescriptorSets_;
    graphic::Pipeline pipeline_;

    std::unique_ptr<graphic::Buffer> pVertexBuffer_;
    std::unique_ptr<graphic::Buffer> pIndexBuffer_;
    std::vector<graphic::Buffer> mvpBuffers_;

    vk::raii::CommandPool vkCommandPool_{nullptr};
    vk::raii::CommandBuffers vkCommandBuffers_{nullptr};

    std::vector<vk::raii::Semaphore> imageAcquiredSemaphores_;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores_;
    std::vector<vk::raii::Fence> drawFences_;
};

} // namespace nae
