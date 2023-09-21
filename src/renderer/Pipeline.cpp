#include "renderer/Pipeline.hpp"

#include "core/App.hpp"
#include "renderer/Descriptor.hpp"
#include "scene/Mesh.hpp"

namespace nae {

Pipeline::Pipeline(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    const auto &device = App::get().getGraphicContext().getDevice();

    // Shader
    vertexShaderModule_ = ShaderModule{device, vertexShaderPath};
    fragmentShaderModule_ = ShaderModule{device, fragmentShaderPath};
    std::array<vk::PipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfos = {
            vk::PipelineShaderStageCreateInfo{vk::PipelineShaderStageCreateFlags{},
                                              vk::ShaderStageFlagBits::eVertex,
                                              *vertexShaderModule_.get(),
                                              "main",
                                              nullptr},
            vk::PipelineShaderStageCreateInfo{vk::PipelineShaderStageCreateFlags{},
                                              vk::ShaderStageFlagBits::eFragment,
                                              *fragmentShaderModule_.get(),
                                              "main",
                                              nullptr}};

    // Vertex input empty for now because hard coded in shader
    auto bindingDescriptions = Mesh::Vertex::getBindingDescriptions();
    auto attributeDescriptions = Mesh::Vertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{vk::PipelineVertexInputStateCreateFlags{},
                                                                              bindingDescriptions,
                                                                              attributeDescriptions};

    // Input assembly
    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{
            vk::PipelineInputAssemblyStateCreateFlags{},
            vk::PrimitiveTopology::eTriangleList};

    // Viewport
    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{vk::PipelineViewportStateCreateFlags{},
                                                                        1,
                                                                        nullptr,
                                                                        1,
                                                                        nullptr};
    std::array<vk::DynamicState, 2> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{vk::PipelineDynamicStateCreateFlags{},
                                                                      dynamicStates};

    // Rasterizer
    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{
            vk::PipelineRasterizationStateCreateFlags{},
            false,
            false,
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack,
            vk::FrontFace::eCounterClockwise,
            false,
            0.0f,
            0.0f,
            0.0f,
            1.0f};

    // Multisampling
    vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{vk::PipelineMultisampleStateCreateFlags{},
                                                                              vk::SampleCountFlagBits::e1,
                                                                              false,
                                                                              1.0f,
                                                                              nullptr,
                                                                              false,
                                                                              false};

    // Depth and stencil
    vk::StencilOpState stencilOpState{vk::StencilOp::eKeep,
                                      vk::StencilOp::eKeep,
                                      vk::StencilOp::eKeep,
                                      vk::CompareOp::eAlways};
    vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{
            vk::PipelineDepthStencilStateCreateFlags{},
            true,
            true,
            vk::CompareOp::eLess,
            false,
            false,
            stencilOpState,
            stencilOpState};

    // Color blending disable for now
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
                                                                            {{0.0f, 0.0f, 0.0f, 0.0f}}};

    // TODO: shader reflect for ubo and push constant layout
    // Push constant range
    vk::PushConstantRange pushConstantRange{vk::ShaderStageFlagBits::eVertex, 0, sizeof(ObjectPushConstant)};

    // Descriptor set layout
    DescriptorSetLayout uboDescriptorSetLayout{
            device,
            {{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}}};
    DescriptorSetLayout samplerDescriptorSetLayout{
            device,
            {{0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}}};

    std::array<vk::DescriptorSetLayout, 2> descriptorSetLayouts = {*uboDescriptorSetLayout.get(),
                                                                   *samplerDescriptorSetLayout.get()};
    // Pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{vk::PipelineLayoutCreateFlags{},
                                                          descriptorSetLayouts,
                                                          pushConstantRange};
    vkPipelineLayout_ = vk::raii::PipelineLayout{device.get(), pipelineLayoutCreateInfo};

    // Pipeline cache
    vkPipelineCache_ = vk::raii::PipelineCache{device.get(), vk::PipelineCacheCreateInfo{}};

    // Pipeline
    const auto &renderPass = App::get().getRenderer().getRenderPass();

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
                                                              *vkPipelineLayout_,
                                                              *renderPass.get(),
                                                              0};
    vkPipeline_ = vk::raii::Pipeline{device.get(), vkPipelineCache_, graphicsPipelineCreateInfo};
}

} // namespace nae
