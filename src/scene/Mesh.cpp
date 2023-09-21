#include "scene/Mesh.hpp"

#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "core/App.hpp"

namespace nae {
std::array<vk::VertexInputBindingDescription, 1> Mesh::Vertex::getBindingDescriptions() {
    return {vk::VertexInputBindingDescription{0, sizeof(Vertex), vk::VertexInputRate::eVertex}};
}

std::vector<vk::VertexInputAttributeDescription> Mesh::Vertex::getAttributeDescriptions() {
    return {vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
            vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)},
            vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
            vk::VertexInputAttributeDescription{3, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord)}};
}

Mesh::Mesh(const std::string &filePath) : filePath_{filePath} {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string error;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &error, filePath.c_str())) {
        throw std::runtime_error(error);
    }

    for (const auto &shape: shapes) {
        for (const auto &index: shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                                   attrib.vertices[3 * index.vertex_index + 1],
                                   attrib.vertices[3 * index.vertex_index + 2]};

                vertex.color = {1.0f, 1.0f, 1.0f};
            }

            if (index.normal_index >= 0) {
                vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                                 attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};
            }

            if (index.texcoord_index >= 0) {
                vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }
            vertices_.push_back(vertex);
            // ignore index for now
        }
    }

    const auto &device = App::get().getGraphicContext().getDevice();
    const auto &vkCommandPool_ = App::get().getRenderer().getVkCommandPool();
    pVertexBuffer_ = std::make_unique<Buffer>(
            device,
            sizeof(Vertex) * vertices_.size(),
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal);
    pVertexBuffer_->copyToDeviceMemory(vkCommandPool_, device.getGraphicQueue(), vertices_);
}

void Mesh::draw(const vk::raii::CommandBuffer &vkCommandBuffer) const {
    // TODO: support index draw
    vkCommandBuffer.bindVertexBuffers(0, {*pVertexBuffer_->get()}, {0});
    vkCommandBuffer.draw(vertices_.size(), 1, 0, 0);
}

} // namespace nae
