#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex;

layout(location = 0) out vec3 normalFrag;
layout(location = 1) out vec3 posFrag;

layout(binding = 0) coherent buffer block0
{
    vec2 h[];
};

layout(binding = 1) coherent buffer block1
{
    vec2 normalX[];
};

layout(binding = 2) coherent buffer block2
{
    vec2 normalZ[];
};

layout(binding = 3) coherent buffer block3
{
    vec2 dx[];
};

layout(binding = 4) coherent buffer block4
{
    vec2 dz[];
};

uniform float lambda;

layout(std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};
uniform mat4 model;

void main()
{
    vec2 texInv = vec2(1.0f - tex.y, tex.x);

    vec3 pos = position + vec3(-lambda * dx[gl_VertexID].x, h[gl_VertexID].x, -lambda * dz[gl_VertexID].x);
    // vec3 pos = position + vec3(0, h[gl_VertexID].x, 0);
    gl_Position = projection * view * model * vec4(pos.x, pos.y, pos.z, 1.0);
    posFrag = vec3(model * vec4(pos.x, pos.y, pos.z, 1.0));
    normalFrag = vec3(normalX[gl_VertexID].x, 1.0f, normalZ[gl_VertexID].x);
    normalFrag = normalize(normalFrag);
}