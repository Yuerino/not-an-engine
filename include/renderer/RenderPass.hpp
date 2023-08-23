#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "core/Device.hpp"

namespace nae {

class Device;

class RenderPass {
public:
    RenderPass(const Device &device,
               vk::Format colorFormat,
               vk::Format depthFormat = vk::Format::eUndefined,
               vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eClear,
               vk::ImageLayout colorFinalLayout = vk::ImageLayout::ePresentSrcKHR);
    explicit RenderPass(std::nullptr_t) {}
    ~RenderPass() = default;

    RenderPass(const RenderPass &) = delete;
    RenderPass &operator=(const RenderPass &) = delete;
    RenderPass(RenderPass &&) noexcept = default;
    RenderPass &operator=(RenderPass &&) noexcept = default;

    [[nodiscard]] const vk::raii::RenderPass &get() const noexcept;

private:
    vk::raii::RenderPass vkRenderPass_{nullptr};
};

} // namespace nae
