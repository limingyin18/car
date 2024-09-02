#include "ActorSkeletalIndirectLod.hpp"

#include "assimp/Animation.hpp"
#include "assimp/ModelSkeletalLod.hpp"
#include "render/Mesh/MeshSkeletal.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Primitive/PrimitiveSkeletalIndirectLod.hpp"
#include "tools/Tool.hpp"

#include "glad/glad.h"
#include <cmath>
#include <cstddef>

using namespace std;

void ActorSkeletalIndirectLod::Init()
{
    model_ = make_shared<ModelSkeletalLod>();
    Actor::Init();
}

void ActorSkeletalIndirectLod::CreatePrimitives()
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
        auto primitive = make_shared<PrimitiveSkeletalIndirectLod>();
        primitive->SetObjectData(object_data_);
        primitive->Init(mesh, shader_, shader_culling_, instance_transforms_, bone_transforms_);
        primitives_.push_back(primitive);
    }
}