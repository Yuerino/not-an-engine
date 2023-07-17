#ifndef NOT_AN_ENGINE_GRAPHIC_PIPELINE_HPP
#define NOT_AN_ENGINE_GRAPHIC_PIPELINE_HPP

#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

[[nodiscard]] vk::raii::Pipeline
createGraphicPipeline(const vk::raii::Device &device,
                      const vk::raii::PipelineCache &pipelineCache,
                      const vk::raii::ShaderModule &vertexShaderModule,
                      const vk::SpecializationInfo *vertexShaderSpecializationInfo,
                      const vk::raii::ShaderModule &fragmentShaderModule,
                      const vk::SpecializationInfo *fragmentShaderSpecializationInfo,
                      uint32_t vertexStride,
                      const std::vector<std::pair<vk::Format, uint32_t>> &vertexInputAttributeFormatOffset,
                      vk::FrontFace frontFace,
                      bool depthBuffered,
                      const vk::raii::PipelineLayout &pipelineLayout,
                      const vk::raii::RenderPass &renderPass);

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_PIPELINE_HPP
