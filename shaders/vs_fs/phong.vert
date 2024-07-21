#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fNormal;
out vec3 fColor;
out vec2 TexCoord;

out vec3 FragPos;


void main()
{
    vec4 position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = projection * view * model * position;

    FragPos = vec3(model * vec4(aPos, 1.0));

    fNormal = mat3(transpose(inverse(model))) * aNormal;
    fColor = aColor;
    TexCoord = aTexCoord;
}