#ifndef NOT_AN_ENGINE_GRAPHIC_DESCRIPTOR_HPP
#define NOT_AN_ENGINE_GRAPHIC_DESCRIPTOR_HPP

#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "graphic/Texture.hpp"

namespace nae::graphic {

[[nodiscard]] vk::raii::DescriptorSetLayout createDescriptorSetLayout(
        const vk::raii::Device &device,
        const std::vector<std::tuple<vk::DescriptorType, uint32_t, vk::ShaderStageFlags>> &bindingData,
        vk::DescriptorSetLayoutCreateFlags flags = {});

[[nodiscard]] vk::raii::DescriptorPool createDescriptorPool(const vk::raii::Device &device,
                                                            const std::vector<vk::DescriptorPoolSize> &poolSizes);

void updateDescriptorSets(
        const vk::raii::Device &device,
        const vk::raii::DescriptorSet &descriptorSet,
        const std::vector<
                std::tuple<vk::DescriptorType, const vk::raii::Buffer &, vk::DeviceSize, const vk::raii::BufferView *>>
                &bufferData,
        const std::vector<TextureData> &textureData,
        uint32_t bindingOffset = 0);

} // namespace nae::graphic

#endif // NOT_AN_ENGINE_GRAPHIC_DESCRIPTOR_HPP
