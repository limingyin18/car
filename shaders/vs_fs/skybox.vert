#version 460

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform Camera 
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

out vec3 localPos;

void main()
{
    localPos = aPos;

    mat4 rotView = mat4(mat3(view)); // remove translation from the view matrix
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}