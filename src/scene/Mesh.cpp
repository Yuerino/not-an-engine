#include "scene/Mesh.hpp"

namespace nae {

Mesh::Mesh(std::unique_ptr<Model> pModel, std::unique_ptr<Texture> pTexture)
    : pModel_{std::move(pModel)}, pTexture_{std::move(pTexture)} {}

} // namespace nae
