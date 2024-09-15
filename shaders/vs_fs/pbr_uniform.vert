#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
// layout(location = 5) in ivec4 boneIds;
// layout(location = 6) in vec4 weights;

layout(std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};
uniform mat4 model;

out vec3 Normal;
out vec2 TexCoords;
out vec3 WorldPos;

void main()
{
    vec4 position = vec4(pos.x, pos.y, pos.z, 1.0);
    gl_Position = projection * view * model * position;

    WorldPos = vec3(model * vec4(pos, 1.0));

    Normal = mat3(transpose(inverse(model))) * norm;
    TexCoords = tex;
}