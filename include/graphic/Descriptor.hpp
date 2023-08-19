#pragma once

#include <functional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "graphic/Buffer.hpp"
#include "graphic/Device.hpp"

namespace nae::graphic {

class DescriptorPool {
public:
    DescriptorPool(const Device &device, const std::vector<vk::DescriptorPoolSize> &poolSizes);
    ~DescriptorPool() = default;

    DescriptorPool(const DescriptorPool &) = delete;
    DescriptorPool &operator=(const DescriptorPool &) = delete;
    DescriptorPool(DescriptorPool &&) = default;
    DescriptorPool &operator=(DescriptorPool &&) = default;

    [[nodiscard]] const vk::raii::DescriptorPool &get() const noexcept;

private:
    std::reference_wrapper<const Device> device_;
    vk::raii::DescriptorPool vkDescriptorPool_{nullptr};
};

class DescriptorSetLayout {
public:
    DescriptorSetLayout(const Device &device, const std::vector<vk::DescriptorSetLayoutBinding> &bindings);
    ~DescriptorSetLayout() = default;

    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout(DescriptorSetLayout &&) = default;
    DescriptorSetLayout &operator=(DescriptorSetLayout &&) = default;

    [[nodiscard]] const vk::raii::DescriptorSetLayout &get() const noexcept;

private:
    vk::raii::DescriptorSetLayout vkDescriptorSetLayout_{nullptr};
};

class DescriptorSets {
public:
    DescriptorSets(const Device &device,
                   const DescriptorPool &descriptorPool,
                   const std::vector<vk::DescriptorSetLayout> &vkDescriptorSetLayouts);
    ~DescriptorSets() = default;

    DescriptorSets(const DescriptorSets &) = delete;
    DescriptorSets &operator=(const DescriptorSets &) = delete;
    DescriptorSets(DescriptorSets &&) = default;
    DescriptorSets &operator=(DescriptorSets &&) = default;

    void update(const std::vector<Buffer> &buffers, uint32_t bindingOffset = 0);

    [[nodiscard]] const vk::raii::DescriptorSets &get() const noexcept;
    [[nodiscard]] vk::raii::DescriptorSet &operator[](size_t idx) noexcept;
    [[nodiscard]] const vk::raii::DescriptorSet &operator[](size_t idx) const noexcept;

private:
    std::reference_wrapper<const Device> device_;
    vk::raii::DescriptorSets vkDescriptorSets_{nullptr};
};

} // namespace nae::graphic
