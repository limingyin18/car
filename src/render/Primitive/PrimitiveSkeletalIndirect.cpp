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
    bone_transforms_buffer_objects_.resize(bone_transforms_.size());
    for (size_t i = 0; i < bone_transforms_.size(); i++)
    {
        glCreateBuffers(1, &bone_transforms_buffer_objects_[i]);
        glNamedBufferStorage(bone_transforms_buffer_objects_[i], bone_transforms_[i].size() * sizeof(glm::mat4),
                             bone_transforms_[i].data(), 0);
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
    for (size_t i = 0; i < bone_transforms_buffer_objects_.size(); i++)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3 + i, bone_transforms_buffer_objects_[i]);
    }
    PrimitiveIndirect::Draw();
}