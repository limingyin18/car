#pragma once

#include "ActorIndirect.hpp"
#include "ActorSkeletal.hpp"

#include "render/Primitive/PrimitiveSkeletalIndirect.hpp"

class ActorSkeletalIndirect : public ActorSkeletal, public ActorIndirect
{
  public:
    void Update() override;

    void SwitchAnimation(uint32_t index) override;

  protected:
    void CreatePrimitives() override;

    void LoadBakedData(uint32_t animation_clip_type) {};
    void SaveBakedData(uint32_t animation_clip_type);
    void InitFrameOffsets();

    std::vector<uint32_t> frame_counts_;
    std::vector<std::vector<glm::mat4>> bone_transforms_;
    std::vector<uint32_t> bone_transforms_buffer_objects_;

    std::vector<ObjectDataAnimation> object_data_;
    std::vector<uint32_t> frame_offsets_;
};