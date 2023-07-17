#include "graphic/FrameBuffer.hpp"

namespace nae::graphic {

std::vector<vk::raii::Framebuffer> createFrameBuffers(const vk::raii::Device &device,
                                                      const vk::raii::RenderPass &renderPass,
                                                      const std::vector<vk::raii::ImageView> &imageViews,
                                                      const vk::raii::ImageView *depthImageViewPtr,
                                                      const vk::Extent2D &extent) {
    vk::ImageView attachments[2];
    attachments[1] = depthImageViewPtr ? **depthImageViewPtr : vk::ImageView{};

    vk::FramebufferCreateInfo framebufferCreateInfo{vk::FramebufferCreateFlags{},
                                                    *renderPass,
                                                    static_cast<uint32_t>(depthImageViewPtr ? 2 : 1),
                                                    attachments,
                                                    extent.width,
                                                    extent.height,
                                                    1};

    std::vector<vk::raii::Framebuffer> frameBuffers;
    frameBuffers.reserve(imageViews.size());

    for (const auto &imageView: imageViews) {
        attachments[0] = *imageView;
        frameBuffers.emplace_back(device, framebufferCreateInfo);
    }

    return frameBuffers;
}

} // namespace nae::graphic
