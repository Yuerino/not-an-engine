#include "core/Scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace nae {

static std::vector<Vertex> loadModel(const std::string &path) {
    std::vector<Vertex> vertices;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string error;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &error, path.c_str())) {
        throw std::runtime_error(error);
    }

    for (const auto &shape: shapes) {
        for (const auto &index: shape.mesh.indices) {
            Vertex vertex{};

            vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                               attrib.vertices[3 * index.vertex_index + 1],
                               attrib.vertices[3 * index.vertex_index + 2]};

            vertex.color = {1.0f, 1.0f, 1.0f};

            vertices.push_back(vertex);
        }
    }

    return vertices;
}

void Scene::onAttach(Renderer &renderer) {
    pRenderer_ = &renderer;
}

BasicScene::BasicScene() {
    pVertices_ = std::make_unique<std::vector<Vertex>>(loadModel("C:/Users/yueri/Documents/new_42.obj"));
}

void BasicScene::onAttach(Renderer &renderer) {
    Scene::onAttach(renderer);
    pVertexBuffer_ = std::make_unique<Buffer>(pRenderer_->loadVertices(*pVertices_));
}

void BasicScene::onUpdate(Time timestep) {
    pRenderer_->beginScene();
    pRenderer_->draw(*pVertexBuffer_, pVertices_->size());
    pRenderer_->endScene();
}

} // namespace nae
