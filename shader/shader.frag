#version 450

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inFragPos;

layout (location = 0) out vec4 outColor;

const vec3 lightPos = vec3(10.0, 10.0, 10.0);
const float ambient = 0.02f;

void main()
{
    vec3 lightDir = normalize(lightPos - inFragPos);

    vec3 normal = normalize(inNormal);

    float lightIntensity = max(dot(normal, lightDir), 0.0);

    outColor = vec4((ambient + lightIntensity) * inColor, 1.0);
}
