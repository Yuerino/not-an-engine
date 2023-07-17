#include "graphic/Descriptor.hpp"

#include <numeric>

#include "util.hpp"

namespace nae::graphic {

vk::raii::DescriptorSetLayout createDescriptorSetLayout(
        const vk::raii::Device &device,
        const std::vector<std::tuple<vk::DescriptorType, uint32_t, vk::ShaderStageFlags>> &bindingData,
        vk::DescriptorSetLayoutCreateFlags flags) {
    std::vector<vk::DescriptorSetLayoutBinding> bindings(bindingData.size());
    for (size_t i = 0; i < bindingData.size(); ++i) {
        bindings[i] = vk::DescriptorSetLayoutBinding(util::checked_cast<uint32_t>(i),
                                                     std::get<0>(bindingData[i]),
                                                     std::get<1>(bindingData[i]),
                                                     std::get<2>(bindingData[i]));
    }
    return {device, {flags, bindings}};
}

vk::raii::DescriptorPool createDescriptorPool(const vk::raii::Device &device,
                                              const std::vector<vk::DescriptorPoolSize> &poolSizes) {
    assert(!poolSizes.empty());
    uint32_t maxSets =
            std::accumulate(poolSizes.begin(), poolSizes.end(), 0, [](uint32_t sum, const vk::DescriptorPoolSize &dps) {
                return sum + dps.descriptorCount;
            });
    assert(0 < maxSets);

    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                                          maxSets,
                                                          poolSizes};
    return {device, descriptorPoolCreateInfo};
}

void updateDescriptorSets(
        const vk::raii::Device &device,
        const vk::raii::DescriptorSet &descriptorSet,
        const std::vector<
                std::tuple<vk::DescriptorType, const vk::raii::Buffer &, vk::DeviceSize, const vk::raii::BufferView *>>
                &bufferData,
        const std::vector<TextureData> &textureData,
        uint32_t bindingOffset) {
    std::vector<vk::DescriptorBufferInfo> bufferInfos;
    bufferInfos.reserve(bufferData.size());

    std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
    writeDescriptorSets.reserve(bufferData.size() + (textureData.empty() ? 0 : 1));
    uint32_t dstBinding = bindingOffset;

    for (const auto &bd: bufferData) {
        bufferInfos.emplace_back(*std::get<1>(bd), 0, std::get<2>(bd));
        vk::BufferView bufferView;
        if (std::get<3>(bd)) {
            bufferView = **std::get<3>(bd);
        }
        writeDescriptorSets.emplace_back(*descriptorSet,
                                         dstBinding++,
                                         0,
                                         1,
                                         std::get<0>(bd),
                                         nullptr,
                                         &bufferInfos.back(),
                                         std::get<3>(bd) ? &bufferView : nullptr);
    }

    std::vector<vk::DescriptorImageInfo> imageInfos;
    if (!textureData.empty()) {
        imageInfos.reserve(textureData.size());
        for (const auto &td: textureData) {
            imageInfos.emplace_back(*td.vkSampler_, *td.imageData_.imageView_, vk::ImageLayout::eShaderReadOnlyOptimal);
        }
        writeDescriptorSets.emplace_back(*descriptorSet,
                                         dstBinding,
                                         0,
                                         util::checked_cast<uint32_t>(imageInfos.size()),
                                         vk::DescriptorType::eCombinedImageSampler,
                                         imageInfos.data(),
                                         nullptr,
                                         nullptr);
    }

    device.updateDescriptorSets(writeDescriptorSets, nullptr);
}

} // namespace nae::graphic
