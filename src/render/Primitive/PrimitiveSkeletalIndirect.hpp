#pragma once

#include "PrimitiveIndirect.hpp"

struct ObjectDataAnimation
{
    uint32_t animation_clip_ = 0;
    uint32_t frame_ = 0;
};

class PrimitiveSkeletalIndirect : public PrimitiveIndirect
{
  public:
    void Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
              const std::shared_ptr<Shader> &shader_culling, const std::vector<glm::mat4> &instance_transforms,
              const std::vector<std::vector<glm::mat4>> &bone_transforms);

    void SetObjectData(const std::vector<ObjectDataAnimation> &object_data)
    {
        object_data_ = object_data;
    }


    void Draw() override;

  protected:
    void CreateObjectDataBuffer() override;
    std::vector<ObjectDataAnimation> object_data_;

    std::vector<std::vector<glm::mat4>> bone_transforms_;
    uint32_t bone_transforms_buffer_object_ = -1;
    std::vector<uint32_t> frame_count_offset_;
    uint32_t frame_count_offset_buffer_object_ = -1;
};