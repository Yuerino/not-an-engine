#pragma once

#include <memory>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "core/Component.hpp"
#include "renderer/Buffer.hpp"

namespace nae {

struct Model {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
        glm::vec2 texCoord;

        [[nodiscard]] static std::array<vk::VertexInputBindingDescription, 1> getBindingDescriptions();
        [[nodiscard]] static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    };

    explicit Model(const std::string &filePath);
    ~Model() = default;

    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;
    Model(Model &&) = default;
    Model &operator=(Model &&) = default;

    [[nodiscard]] const std::string &getFilePath() const noexcept { return filePath_; }
    [[nodiscard]] const std::vector<Vertex> &getVertices() const noexcept { return vertices_; }
    [[nodiscard]] const std::vector<uint32_t> &getIndices() const noexcept { return indices_; }
    [[nodiscard]] Buffer &getVertexBuffer() const noexcept { return *pVertexBuffer_; }

private:
    std::string filePath_;
    std::vector<Vertex> vertices_{};
    std::vector<uint32_t> indices_{};
    std::unique_ptr<Buffer> pVertexBuffer_{};
};

class Entity {
public:
    Entity() = default;
    explicit Entity(std::unique_ptr<Model> pModel) : pModel_{std::move(pModel)} {}
    ~Entity() = default;

    Entity(const Entity &) = delete;
    Entity &operator=(const Entity &) = delete;
    Entity(Entity &&) = default;
    Entity &operator=(Entity &&) = default;

    [[nodiscard]] bool hasModel() const noexcept { return pModel_ != nullptr; }
    [[nodiscard]] Model &getModel() const noexcept { return *pModel_; }
    [[nodiscard]] const TransformComponent &getTransform() const noexcept { return transform_; }

    void setModel(std::unique_ptr<Model> pModel) { pModel_ = std::move(pModel); }
    void setTransform(const TransformComponent &transform) { transform_ = transform; }

private:
    TransformComponent transform_{};
    std::unique_ptr<Model> pModel_{};
};

} // namespace nae
