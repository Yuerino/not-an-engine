#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "graphic/Device.hpp"

namespace nae::graphic {

class RenderPass {
public:
    RenderPass(const Device &device,
               vk::Format colorFormat,
               vk::Format depthFormat,
               vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eClear,
               vk::ImageLayout colorFinalLayout = vk::ImageLayout::ePresentSrcKHR);
    explicit RenderPass(std::nullptr_t) {}

    RenderPass(const RenderPass &) = delete;
    RenderPass &operator=(const RenderPass &) = delete;
    RenderPass(RenderPass &&) noexcept = default;
    RenderPass &operator=(RenderPass &&) noexcept = default;

    [[nodiscard]] const vk::raii::RenderPass &get() const noexcept;

private:
    vk::raii::RenderPass vkRenderPass_{nullptr};
};

} // namespace nae::graphic
