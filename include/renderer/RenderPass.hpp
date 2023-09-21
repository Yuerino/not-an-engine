#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace nae {

class RenderPass {
public:
    RenderPass(vk::Format colorFormat,
               vk::Format depthFormat = vk::Format::eUndefined,
               vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eClear,
               vk::ImageLayout colorFinalLayout = vk::ImageLayout::ePresentSrcKHR);

    [[nodiscard]] const vk::raii::RenderPass &get() const noexcept;

private:
    vk::raii::RenderPass vkRenderPass_{nullptr};
};

} // namespace nae
