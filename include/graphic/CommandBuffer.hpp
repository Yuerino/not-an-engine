#ifndef NOT_AN_ENGINE_GRAPHIC_COMMANDBUFFER_HPP
#define NOT_AN_ENGINE_GRAPHIC_COMMANDBUFFER_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "graphic/Device.hpp"

namespace nae::graphic {

[[nodiscard]] vk::raii::CommandBuffer createCommandBuffer(const vk::raii::Device &device,
                                                          const vk::raii::CommandPool &commandPool);

void setImageLayout(const vk::raii::CommandBuffer &commandBuffer,
                    vk::Image image,
                    vk::Format format,
                    vk::ImageLayout oldImageLayout,
                    vk::ImageLayout newImageLayout);

template<typename Func>
void oneTimeSubmit(const vk::raii::CommandBuffer &commandBuffer, const vk::raii::Queue &queue, const Func &func) {
    commandBuffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    func(commandBuffer);
    commandBuffer.end();
    vk::SubmitInfo submitInfo{nullptr, nullptr, *commandBuffer};
    queue.submit(submitInfo, nullptr);
    queue.waitIdle();
}

template<typename Func>
void oneTimeSubmit(const Device &device,
                   const vk::raii::CommandPool &commandPool,
                   const vk::raii::Queue &queue,
                   const Func &func) {
    vk::raii::CommandBuffers commandBuffers{device.get(), {*commandPool, vk::CommandBufferLevel::ePrimary, 1}};
    oneTimeSubmit(commandBuffers.front(), queue, func);
}

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_COMMANDBUFFER_HPP
