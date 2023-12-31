#version 450

layout (set = 0, binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
} cameraResources;

layout (push_constant) uniform PushConstant {
    mat4 transform;
} objectResources;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexCoord;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec3 fragPosition;
layout (location = 3) out vec2 fragTexCoord;

void main() {
    gl_Position = cameraResources.proj * cameraResources.view * objectResources.transform * vec4(inPosition, 1.0);

    fragColor = inColor;

    // uniform scaling
    fragNormal = inNormal;

    // non-uniform scaling
    //     fragNormal = mat3(transpose(inverse(mvpMatrices.model))) * inNormal;

    fragPosition = vec3(objectResources.transform * vec4(inPosition, 1.0));

    fragTexCoord = inTexCoord;
}
