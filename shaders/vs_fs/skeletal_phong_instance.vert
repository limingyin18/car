#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

layout(std140, binding = 0) uniform Camera 
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};
uniform mat4 model;
layout(std430, binding = 1) buffer InstanceBuffer
{
    mat4 modelMatrices[];
};
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
	

out vec3 fNormal;
out vec2 TexCoord;
out vec3 FragPos;


	
void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
        totalNormal += localNormal * weights[i];
    }
		
    FragPos = vec3(modelMatrices[gl_InstanceID] * model * totalPosition);
    fNormal = mat3(transpose(inverse(modelMatrices[gl_InstanceID] * model))) * totalNormal;
    mat4 viewModel = view * modelMatrices[gl_InstanceID] * model;
    gl_Position =  projection * viewModel * totalPosition;
    TexCoord = tex;
}