#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

namespace nae {

class SolidColorMaterial final : public Material {
public:
    SolidColorMaterial(std::string name, const glm::vec3 &color) : Material{std::move(name)}, color_{color} {}

    void bind(const vk::raii::CommandBuffer &vkCommandBuffer) const final;

    void setColor(const glm::vec3 &color) { color_ = color; }
    [[nodiscard]] const glm::vec3 &getColor() const noexcept { return color_; }

private:
    glm::vec3 color_;
};

} // namespace nae
