#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

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
} // namespace nae
