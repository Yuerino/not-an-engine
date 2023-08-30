#include "core/Scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "core/App.hpp"

namespace nae {

Scene::Scene() : pRenderer_{&App::get().getRenderer()} {}

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

            vertices.push_back(vertex);
        }
    }

    return vertices;
}

BasicScene::BasicScene()
    : pCamera_{std::make_shared<Camera>(App::get().getWindow().getAspectRatio())},
      pCameraController_{std::make_unique<CameraController>(pCamera_)},
      pVertices_{std::make_unique<std::vector<Vertex>>(loadModel("C:/Users/yueri/Documents/new_42.obj"))},
      pVertexBuffer_{std::make_unique<Buffer>(pRenderer_->loadVertices(*pVertices_))} {}

void BasicScene::onAttach() {}

void BasicScene::onResize(float aspectRatio) {
    pCameraController_->onResize(aspectRatio);
}

void BasicScene::onUpdate(Time timestep) {
    pCameraController_->onUpdate(timestep);
    pRenderer_->beginScene(*pCamera_);
    pRenderer_->draw(*pVertexBuffer_, pVertices_->size());
    pRenderer_->endScene();
}

} // namespace nae
