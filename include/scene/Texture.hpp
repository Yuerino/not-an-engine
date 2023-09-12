#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "renderer/Descriptor.hpp"
#include "renderer/Image.hpp"

namespace nae {

class Texture {
public:
    explicit Texture(const vk::Extent2D &extent_ = {256, 256},
                     vk::ImageUsageFlags usageFlags = {},
                     vk::FormatFeatureFlags formatFeatureFlags = {},
                     bool forceStaging = false);
    ~Texture() = default;

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;
    Texture(Texture &&) = default;
    Texture &operator=(Texture &&) = default;

    void setTexels(const std::string &texelPath);

    [[nodiscard]] const DescriptorSets &getDescriptorSets() const noexcept { return *pDescriptorSets_; };

private:
    vk::Format format_;
    vk::Extent2D extent_;

    bool needStaging_{false};
    std::unique_ptr<Image> pTextureImage_;
    std::unique_ptr<DescriptorSets> pDescriptorSets_;
};

} // namespace nae
