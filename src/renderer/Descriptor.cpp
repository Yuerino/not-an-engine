#include "renderer/Descriptor.hpp"

#include <numeric>

namespace nae {

DescriptorPool::DescriptorPool(const Device &device, const std::vector<vk::DescriptorPoolSize> &poolSizes)
    : device_{device} {
    assert(not poolSizes.empty());
    uint32_t maxSets =
            std::accumulate(poolSizes.begin(), poolSizes.end(), 0, [](uint32_t sum, const vk::DescriptorPoolSize &dps) {
                return sum + dps.descriptorCount;
            });
    assert(maxSets > 0);

    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                                          maxSets,
                                                          poolSizes};
    vkDescriptorPool_ = vk::raii::DescriptorPool{device.get(), descriptorPoolCreateInfo};
}

const vk::raii::DescriptorPool &DescriptorPool::get() const noexcept {
    return vkDescriptorPool_;
}

DescriptorSetLayout::DescriptorSetLayout(const Device &device,
                                         const std::vector<vk::DescriptorSetLayoutBinding> &bindings)
    : vkDescriptorSetLayout_{device.get(), {vk::DescriptorSetLayoutCreateFlags{}, bindings}} {}

const vk::raii::DescriptorSetLayout &DescriptorSetLayout::get() const noexcept {
    return vkDescriptorSetLayout_;
}

DescriptorSets::DescriptorSets(const Device &device,
                               const DescriptorPool &descriptorPool,
                               const std::vector<vk::DescriptorSetLayout> &vkDescriptorSetLayouts)
    : device_{device},
      vkDescriptorSets_{vk::raii::DescriptorSets{device.get(), {*descriptorPool.get(), vkDescriptorSetLayouts}}} {}

void DescriptorSets::update(const std::vector<Buffer> &buffers, uint32_t bindingOffset) {
    assert(buffers.size() == vkDescriptorSets_.size());
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
    writeDescriptorSets.reserve(buffers.size());

    for (size_t i = 0; i < buffers.size(); ++i) {
        vk::DescriptorBufferInfo bufferInfo{buffers[i].getBufferInfo()};
        writeDescriptorSets.emplace_back(*vkDescriptorSets_[i],
                                         bindingOffset,
                                         0,
                                         1,
                                         vk::DescriptorType::eUniformBuffer,
                                         nullptr,
                                         &bufferInfo);
    }

    device_.get().get().updateDescriptorSets(writeDescriptorSets, nullptr);
}

const vk::raii::DescriptorSets &DescriptorSets::get() const noexcept {
    return vkDescriptorSets_;
}

vk::raii::DescriptorSet &DescriptorSets::operator[](size_t idx) noexcept {
    return vkDescriptorSets_[idx];
}

const vk::raii::DescriptorSet &DescriptorSets::operator[](size_t idx) const noexcept {
    return vkDescriptorSets_[idx];
}

} // namespace nae::graphic
