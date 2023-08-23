#version 450

layout (binding = 0) uniform MvpMatrices {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvpMatrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;

void main() {
    gl_Position = mvpMatrices.proj * mvpMatrices.view * mvpMatrices.model * vec4(inPosition, 1.0);
    outColor = inColor;
}
