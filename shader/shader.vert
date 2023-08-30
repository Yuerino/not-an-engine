#version 450

layout (binding = 0) uniform MvpMatrices {
    mat4 view;
    mat4 proj;
} mvpMatrices;

layout (push_constant) uniform PushConstants {
    mat4 model;
} pushConstants;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec3 fragPosition;

void main() {
    gl_Position = mvpMatrices.proj * mvpMatrices.view * pushConstants.model * vec4(inPosition, 1.0);
    fragColor = inColor;

    // uniform scaling
    fragNormal = inNormal;

    // non-uniform scaling
    //     fragNormal = mat3(transpose(inverse(mvpMatrices.model))) * inNormal;

    fragPosition = vec3(pushConstants.model * vec4(inPosition, 1.0));
}
