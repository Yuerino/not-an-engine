#include "graphic/RenderPass.hpp"

#include <vector>

namespace nae::graphic {

RenderPass::RenderPass(const Device &device,
                       vk::Format colorFormat,
                       vk::Format depthFormat,
                       vk::AttachmentLoadOp loadOp,
                       vk::ImageLayout colorFinalLayout) {
    assert(colorFormat != vk::Format::eUndefined);

    std::vector<vk::AttachmentDescription> attachmentDescriptions;
    attachmentDescriptions.emplace_back(vk::AttachmentDescriptionFlags{},
                                        colorFormat,
                                        vk::SampleCountFlagBits::e1,
                                        loadOp,
                                        vk::AttachmentStoreOp::eStore,
                                        vk::AttachmentLoadOp::eDontCare,
                                        vk::AttachmentStoreOp::eDontCare,
                                        vk::ImageLayout::eUndefined,
                                        colorFinalLayout);

    if (depthFormat != vk::Format::eUndefined) {
        attachmentDescriptions.emplace_back(vk::AttachmentDescriptionFlags{},
                                            depthFormat,
                                            vk::SampleCountFlagBits::e1,
                                            loadOp,
                                            vk::AttachmentStoreOp::eDontCare,
                                            vk::AttachmentLoadOp::eDontCare,
                                            vk::AttachmentStoreOp::eDontCare,
                                            vk::ImageLayout::eUndefined,
                                            vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    vk::AttachmentReference colorAttachment{0, vk::ImageLayout::eColorAttachmentOptimal};
    vk::AttachmentReference depthAttachment{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    vk::SubpassDescription subPassDescription{vk::SubpassDescriptionFlags{},
                                              vk::PipelineBindPoint::eGraphics,
                                              {},
                                              colorAttachment,
                                              {},
                                              (depthFormat != vk::Format::eUndefined) ? &depthAttachment : nullptr};

    vk::RenderPassCreateInfo renderPassCreateInfo{vk::RenderPassCreateFlags{},
                                                  attachmentDescriptions,
                                                  subPassDescription};
    vkRenderPass_ = vk::raii::RenderPass{device.get(), renderPassCreateInfo};
}

const vk::raii::RenderPass &RenderPass::get() const noexcept {
    return vkRenderPass_;
}

} // namespace nae::graphic
