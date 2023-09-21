#pragma once

#include "renderer/Buffer.hpp"
#include "renderer/Descriptor.hpp"
#include "scene/Camera.hpp"

namespace nae {

class FreeCamera final : public Camera {
public:
    struct CameraUniformBuffer {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

public:
    FreeCamera();

    void onUpdate(Time timestep) final;

    void bind(const vk::raii::CommandBuffer &commandBuffer) const final;

private:
    std::vector<Buffer> uboBuffers_;
    std::unique_ptr<DescriptorSets> pDescriptorSets_;
};

} // namespace nae
