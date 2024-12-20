#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

layout(std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};
uniform mat4 model;

struct CameraCullingData
{
    mat4 view;
    float fov;
    float aspect;
    float znear, zfar;

    vec4 spherebounds;
    uint drawCount;
};

layout(std140, binding = 3) uniform CameraCullingDataBuffer
{
    CameraCullingData cullData;
};

layout(std430, binding = 0) buffer InstanceBuffer
{
    mat4 modelMatrices[];
};
layout(std430, binding = 1) buffer InstanceIDBuffer
{
    uint ids[];
};

struct ObjectData
{
    uint clip;
    uint frame;
};
layout(std430, binding = 2) buffer ObjectDataBuffer
{
    ObjectData objectData[];
};

layout(std430, binding = 3) buffer BonesBuffer1
{
    mat4 BonesMatrices1[];
};

layout(std430, binding = 4) buffer FrameCount
{
    uint frameCount[];
};

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

void main()
{
    uint instance_id = ids[gl_DrawID * cullData.drawCount + gl_InstanceID];

    vec4 totalPosition = vec4(0.0f);

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (boneIds[i] == -1)
            continue;
        if (boneIds[i] >= MAX_BONES)
        {
            totalPosition = vec4(pos, 1.0f);
            break;
        }

        mat4 BoneMatrices = mat4(1.0f);
        uint index = objectData[instance_id].frame * MAX_BONES + boneIds[i];
        uint frameClipOffset = frameCount[objectData[instance_id].clip];
        BoneMatrices = BonesMatrices1[frameClipOffset + index];

        vec4 localPosition = BoneMatrices * vec4(pos, 1.0f);
        totalPosition += localPosition * weights[i];
    }

    gl_Position = modelMatrices[instance_id] * totalPosition;
}