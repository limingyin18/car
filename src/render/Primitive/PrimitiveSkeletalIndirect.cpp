#include "PrimitiveSkeletalIndirect.hpp"
#include "PrimitiveIndirect.hpp"

#include <glad/glad.h>

void PrimitiveSkeletalIndirect::Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
                                     const std::shared_ptr<Shader> &shader_culling,
                                     const std::vector<glm::mat4> &instance_transforms,
                                     const std::vector<std::vector<glm::mat4>> &bone_transforms)
{
    PrimitiveIndirect::Init(mesh, shader, shader_culling, instance_transforms);
    bone_transforms_ = bone_transforms;
    frame_count_offset_.resize(bone_transforms_.size() + 1);
    frame_count_offset_[0] = 0;
    for (size_t i = 1; i < bone_transforms_.size() + 1; i++)
    {
        frame_count_offset_[i] = frame_count_offset_[i - 1] + bone_transforms_[i - 1].size();
    }
    glCreateBuffers(1, &frame_count_offset_buffer_object_);
    glNamedBufferStorage(frame_count_offset_buffer_object_, frame_count_offset_.size() * sizeof(uint32_t),
                         frame_count_offset_.data(), 0);

    glCreateBuffers(1, &bone_transforms_buffer_object_);
    glNamedBufferStorage(bone_transforms_buffer_object_, frame_count_offset_.back() * sizeof(glm::mat4), 0,
                         GL_DYNAMIC_STORAGE_BIT);
    for (size_t i = 0; i < bone_transforms_.size(); i++)
    {
        glNamedBufferSubData(bone_transforms_buffer_object_, frame_count_offset_[i] * sizeof(glm::mat4),
                             bone_transforms_[i].size() * sizeof(glm::mat4), bone_transforms_[i].data());
    }
}

void PrimitiveSkeletalIndirect::CreateObjectDataBuffer()
{
    glCreateBuffers(1, &object_data_buffer_);
    glNamedBufferStorage(object_data_buffer_, object_data_.size() * sizeof(ObjectDataAnimation), object_data_.data(),
                         GL_DYNAMIC_STORAGE_BIT);
}

void PrimitiveSkeletalIndirect::Draw()
{
    glNamedBufferSubData(object_data_buffer_, 0, object_data_.size() * sizeof(ObjectDataAnimation),
                         object_data_.data());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, bone_transforms_buffer_object_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, frame_count_offset_buffer_object_);
    // for (size_t i = 0; i < bone_transforms_buffer_objects_.size(); i++)
    // {
    //     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3 + i, bone_transforms_buffer_objects_[i]);
    // }
    PrimitiveIndirect::Draw();
}