#ifndef NOT_AN_ENGINE_GRAPHIC_RENDERPASS_HPP
#define NOT_AN_ENGINE_GRAPHIC_RENDERPASS_HPP

#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

[[nodiscard]] vk::raii::RenderPass createRenderPass(const vk::raii::Device &device,
                                                    vk::Format colorFormat,
                                                    vk::Format depthFormat,
                                                    vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eClear,
                                                    vk::ImageLayout colorFinalLayout = vk::ImageLayout::ePresentSrcKHR);

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_RENDERPASS_HPP
