#include "ActorSkeletalIndirect.hpp"

#include "assimp/Animation.hpp"
#include "assimp/Animator.hpp"
#include "render/Mesh/MeshSkeletal.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Primitive/PrimitiveSkeletalIndirect.hpp"
#include "tools/Tool.hpp"

#include "glad/glad.h"
#include <cmath>
#include <cstddef>

using namespace std;

void ActorSkeletalIndirect::SaveBakedData(uint32_t animation_clip_type)
{
    auto mesh = dynamic_pointer_cast<MeshSkeletal>(meshes_[0]);

    size_t vertices_size = mesh->GetVertices().size();

    bone_transforms_[animation_clip_type].resize((frame_counts_[animation_clip_type]) * MAX_BONES);
    float ticks_per_second = animations_[animation_clip_type]->GetTicksPerSecond();
    float dt = 10.0f / ticks_per_second;

    animator_->PlayAnimation(animations_[animation_clip_type].get());
    // bake animation
    for (int i = 0; i <= frame_counts_[animation_clip_type] / 10; i++)
    {
        if (i == 0)
        {
            animator_->UpdateAnimation(0);
        }
        else
        {
            animator_->UpdateAnimation(dt);
        }
        auto transforms = animator_->GetFinalBoneMatrices();

        for (int j = 0; j < MAX_BONES; j++)
        {
            bone_transforms_[animation_clip_type][i * MAX_BONES + j] = transforms[j];
        }
    }
    // size_t size_bones_baked = sizeof(glm::mat4) * bone_transforms_[animation_clip_type].size();
    // glCreateBuffers(1, &bone_transforms_buffer_objects_[animation_clip_type]);
    // glNamedBufferStorage(bone_transforms_buffer_objects_[animation_clip_type], size_bones_baked,
    //                      bone_transforms_[animation_clip_type].data(), 0);
}

void ActorSkeletalIndirect::CreatePrimitives()
{
    LoadAnimations();

    object_data_.resize(instance_transforms_.size());
    frame_counts_.resize(animations_.size());
    for (size_t i = 0; i < animations_.size(); i++)
    {
        frame_counts_[i] = animations_[i]->GetDuration();
    }
    InitFrameOffsets();
    bone_transforms_.resize(animations_.size());
    bone_transforms_buffer_objects_.resize(animations_.size());

    for (size_t i = 0; i < animations_.size(); i++)
    {
        SaveBakedData(i);
    }

    for (auto &mesh : meshes_)
    {
        auto primitive = make_shared<PrimitiveSkeletalIndirect>();
        primitive->SetObjectData(object_data_);
        primitive->Init(mesh, shader_, shader_culling_, instance_transforms_, bone_transforms_);
        primitives_.push_back(primitive);
    }
}

void ActorSkeletalIndirect::Update()
{
    for (auto &object_data : object_data_)
    {
        // object_data.animation_clip_ = 0;
        object_data.frame_ = object_data.frame_ + 1;
        object_data.frame_ = object_data.frame_ % (frame_counts_[object_data.animation_clip_] / 10 + 1);
    }

    for (size_t i = 0; i < primitives_.size(); i++)
    {
        auto primitive = dynamic_pointer_cast<PrimitiveSkeletalIndirect>(primitives_[i]);
        primitive->SetObjectData(object_data_);
    }
    ActorIndirect::Update();
}

void ActorSkeletalIndirect::SwitchAnimation(uint32_t index)
{
    for (size_t i = 0; i < object_data_.size(); i++)
    {
        index = std::round(Tool::GetInstance().RandomFloat() * (animations_.size() - 1));
        object_data_[i].animation_clip_ = index;
        object_data_[i].frame_ = frame_offsets_[i];
        object_data_[i].frame_ = object_data_[i].frame_ % (frame_counts_[object_data_[i].animation_clip_] / 10 + 1);
    }
}

void ActorSkeletalIndirect::InitFrameOffsets()
{
    frame_offsets_.resize(instance_transforms_.size());
    for (auto &frame_offset : frame_offsets_)
    {
        frame_offset = static_cast<uint32_t>(static_cast<float>(frame_counts_[0]) * Tool::GetInstance().RandomFloat());
    }

    for (size_t i = 0; i < object_data_.size(); i++)
    {
        object_data_[i].animation_clip_ = 0;
        object_data_[i].frame_ = frame_offsets_[i];
        object_data_[i].frame_ = object_data_[i].frame_ % (frame_counts_[object_data_[i].animation_clip_] / 10 + 1);
    }
}