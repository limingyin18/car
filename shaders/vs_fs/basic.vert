#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    vec4 position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = projection * view * model * position;

    ourColor = aColor;
    TexCoord = aTexCoord;
}