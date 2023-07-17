#include "graphic/Pipeline.hpp"

namespace nae::graphic {

vk::raii::Pipeline
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
                      const vk::raii::RenderPass &renderPass) {
    std::array<vk::PipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfos = {
            vk::PipelineShaderStageCreateInfo{vk::PipelineShaderStageCreateFlags{},
                                              vk::ShaderStageFlagBits::eVertex,
                                              *vertexShaderModule,
                                              "main",
                                              vertexShaderSpecializationInfo},
            vk::PipelineShaderStageCreateInfo{vk::PipelineShaderStageCreateFlags{},
                                              vk::ShaderStageFlagBits::eFragment,
                                              *fragmentShaderModule,
                                              "main",
                                              fragmentShaderSpecializationInfo}};

    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
    vk::VertexInputBindingDescription vertexInputBindingDescription{0, vertexStride};

    if (0 < vertexStride) {
        vertexInputAttributeDescriptions.reserve(vertexInputAttributeFormatOffset.size());
        for (uint32_t i = 0; i < vertexInputAttributeFormatOffset.size(); ++i) {
            vertexInputAttributeDescriptions.emplace_back(i,
                                                          0,
                                                          vertexInputAttributeFormatOffset[i].first,
                                                          vertexInputAttributeFormatOffset[i].second);
        }
        pipelineVertexInputStateCreateInfo.setVertexBindingDescriptions(vertexInputBindingDescription);
        pipelineVertexInputStateCreateInfo.setVertexAttributeDescriptions(vertexInputAttributeDescriptions);
    }

    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{
            vk::PipelineInputAssemblyStateCreateFlags{},
            vk::PrimitiveTopology::eTriangleList};

    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{vk::PipelineViewportStateCreateFlags{},
                                                                        1,
                                                                        nullptr,
                                                                        1,
                                                                        nullptr};

    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{
            vk::PipelineRasterizationStateCreateFlags{},
            false,
            false,
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack,
            frontFace,
            false,
            0.0f,
            0.0f,
            0.0f,
            1.0f};

    vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{vk::PipelineMultisampleStateCreateFlags{},
                                                                              vk::SampleCountFlagBits::e1};

    vk::StencilOpState stencilOpState{vk::StencilOp::eKeep,
                                      vk::StencilOp::eKeep,
                                      vk::StencilOp::eKeep,
                                      vk::CompareOp::eAlways};
    vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{
            vk::PipelineDepthStencilStateCreateFlags{},
            depthBuffered,
            depthBuffered,
            vk::CompareOp::eLessOrEqual,
            false,
            false,
            stencilOpState,
            stencilOpState};

    vk::ColorComponentFlags colorComponentFlags{vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
    vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{false,
                                                                            vk::BlendFactor::eZero,
                                                                            vk::BlendFactor::eZero,
                                                                            vk::BlendOp::eAdd,
                                                                            vk::BlendFactor::eZero,
                                                                            vk::BlendFactor::eZero,
                                                                            vk::BlendOp::eAdd,
                                                                            colorComponentFlags};
    vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{vk::PipelineColorBlendStateCreateFlags{},
                                                                            false,
                                                                            vk::LogicOp::eNoOp,
                                                                            pipelineColorBlendAttachmentState,
                                                                            {{1.0f, 1.0f, 1.0f, 1.0f}}};

    std::array<vk::DynamicState, 2> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{vk::PipelineDynamicStateCreateFlags{},
                                                                      dynamicStates};

    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{vk::PipelineCreateFlags{},
                                                              pipelineShaderStageCreateInfos,
                                                              &pipelineVertexInputStateCreateInfo,
                                                              &pipelineInputAssemblyStateCreateInfo,
                                                              nullptr,
                                                              &pipelineViewportStateCreateInfo,
                                                              &pipelineRasterizationStateCreateInfo,
                                                              &pipelineMultisampleStateCreateInfo,
                                                              &pipelineDepthStencilStateCreateInfo,
                                                              &pipelineColorBlendStateCreateInfo,
                                                              &pipelineDynamicStateCreateInfo,
                                                              *pipelineLayout,
                                                              *renderPass};

    return {device, pipelineCache, graphicsPipelineCreateInfo};
}

} // namespace nae::graphic
