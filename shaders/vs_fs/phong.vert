#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;

layout(std140, binding = 0) uniform Camera 
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};
uniform mat4 model;

out vec3 fNormal;
out vec2 TexCoord;
out vec3 FragPos;


void main()
{
    vec4 position = vec4(pos.x, pos.y, pos.z, 1.0);
    gl_Position = projection * view * model * position;

    FragPos = vec3(model * vec4(pos, 1.0));

    fNormal = mat3(transpose(inverse(model))) * norm;
    TexCoord = tex;
}