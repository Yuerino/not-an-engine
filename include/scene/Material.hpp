#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan_raii.hpp>

#include "renderer/Pipeline.hpp"

namespace nae {

class Material {
public:
    Material(std::string name, std::unique_ptr<Pipeline> pPipeline = nullptr)
        : name_{std::move(name)}, pPipeline_{std::move(pPipeline)} {}
    virtual ~Material() = default;

    virtual void bind(const vk::raii::CommandBuffer &vkCommandBuffer) const = 0;

    void setPipeline(std::unique_ptr<Pipeline> pPipeline) noexcept { pPipeline_ = std::move(pPipeline); }

    [[nodiscard]] const std::string &getName() const noexcept { return name_; }
    [[nodiscard]] Pipeline *getPipeline() const noexcept { return pPipeline_.get(); }

private:
    std::string name_;
    std::unique_ptr<Pipeline> pPipeline_;
};

} // namespace nae
