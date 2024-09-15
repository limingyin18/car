#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
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
out mat3 TBN;

void main()
{
    vec4 position = vec4(pos.x, pos.y, pos.z, 1.0);
    gl_Position = projection * view * model * position;

    WorldPos = vec3(model * vec4(pos, 1.0));

    Normal = mat3(transpose(inverse(model))) * norm;
    TexCoords = tex;

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    // vec3 B = normalize(vec3(modelMatrices[instance_id] * vec4(totalBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(Normal, 0.0)));

    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);
}