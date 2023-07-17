#ifndef NOT_AN_ENGINE_GRAPHIC_FRAMEBUFFER_HPP
#define NOT_AN_ENGINE_GRAPHIC_FRAMEBUFFER_HPP

#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

[[nodiscard]] std::vector<vk::raii::Framebuffer> createFrameBuffers(const vk::raii::Device &device,
                                                                    const vk::raii::RenderPass &renderPass,
                                                                    const std::vector<vk::raii::ImageView> &imageViews,
                                                                    const vk::raii::ImageView *depthImageViewPtr,
                                                                    const vk::Extent2D &extent);
} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_FRAMEBUFFER_HPP
