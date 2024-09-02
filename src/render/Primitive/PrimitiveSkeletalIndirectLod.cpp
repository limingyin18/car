#include "PrimitiveSkeletalIndirectLod.hpp"

#include "PrimitiveInstance.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/MeshSkeletalLod.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Shader/Shader.hpp"
#include <cstdint>

void PrimitiveSkeletalIndirectLod::Draw()
{
    if (mesh_ == nullptr || shader_ == nullptr)
    {
        spdlog::error("Primitive::Draw: mesh or shader is nullptr");
        return;
    }
    // culling pass
    {
        camera_culling_data_.view = camera_->GetView();
        camera_culling_data_.fov = glm::radians(camera_->GetFov());
        camera_culling_data_.aspect = camera_->GetAspect();
        camera_culling_data_.znear = camera_->GetNear();
        camera_culling_data_.zfar = camera_->GetFar();

        glNamedBufferSubData(camera_culling_data_buffer_, 0, sizeof(CameraCullingData), &camera_culling_data_);

        for (size_t i = 0; i < indirect_commands_.size(); i++)
        {
            indirect_commands_[i].instanceCount = 0;
        }
        glNamedBufferSubData(indirect_buffer_, 0, indirect_commands_.size() * sizeof(DrawElementsIndirectCommand),
                             indirect_commands_.data());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instance_transforms_buffer_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instance_id_buffer_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indirect_buffer_);
        glBindBufferBase(GL_UNIFORM_BUFFER, 3, camera_culling_data_buffer_);
        shader_culling_->use();
        glDispatchCompute(instance_transforms_.size(), 1, 1);
        glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    }

    // draw pass
    {
        shader_->use();
        shader_->setMat4("model", mesh_->GetModel());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instance_transforms_buffer_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instance_id_buffer_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, object_data_buffer_);
        glNamedBufferSubData(object_data_buffer_, 0, object_data_.size() * sizeof(ObjectDataAnimation),
                             object_data_.data());
        for (size_t i = 0; i < bone_transforms_buffer_objects_.size(); i++)
        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3 + i, bone_transforms_buffer_objects_[i]);
        }
        glBindBufferBase(GL_UNIFORM_BUFFER, 3, camera_culling_data_buffer_);

        auto &textures = mesh_->GetTextures();
        if (textures.size() > 0)
        {
            glBindTextureUnit(0, textures[0].id);
        }

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_buffer_);

        glBindVertexArray(mesh_->GetVAO());
        auto mesh = std::dynamic_pointer_cast<MeshSkeletalLod>(mesh_);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, mesh->GetLods().size(), 0);
    }
}

void PrimitiveSkeletalIndirectLod::CreateIndirectCommandBuffer()
{
    auto mesh = std::dynamic_pointer_cast<MeshSkeletalLod>(mesh_);
    auto lods = mesh->GetLods();
    uint32_t index_offset = 0;
    for (int i = 0; i < lods.size(); i++)
    {
        DrawElementsIndirectCommand command;
        command.count = lods[i];
        command.instanceCount = 0;
        command.baseInstance = i * instance_transforms_.size();
        command.baseVertex = 0;
        command.firstIndex = index_offset;
        indirect_commands_.push_back(command);
        index_offset += lods[i];
    }

    glCreateBuffers(1, &indirect_buffer_);
    glNamedBufferStorage(indirect_buffer_, indirect_commands_.size() * sizeof(DrawElementsIndirectCommand),
                         indirect_commands_.data(), GL_DYNAMIC_STORAGE_BIT);
}

void PrimitiveSkeletalIndirectLod::CreateInstanceIDBuffer()
{
    auto mesh = std::dynamic_pointer_cast<MeshSkeletalLod>(mesh_);
    instance_id_data_.resize(mesh->GetLods().size() * instance_transforms_.size());

    glCreateBuffers(1, &instance_id_buffer_);
    glNamedBufferStorage(instance_id_buffer_, instance_id_data_.size() * sizeof(InstanceIDData),
                         instance_id_data_.data(), GL_DYNAMIC_STORAGE_BIT);
}