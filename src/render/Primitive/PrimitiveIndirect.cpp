#include "PrimitiveIndirect.hpp"

#include "render/Shader/Shader.hpp"

#include "PrimitiveInstance.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"

void PrimitiveIndirect::Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
                             const std::shared_ptr<Shader> &shader_culling,
                             const std::vector<glm::mat4> &instance_transforms)
{
    shader_culling_ = shader_culling;
    PrimitiveInstance::Init(mesh, shader, instance_transforms);
    CreateObjectDataBuffer();
    CreateCameraCullingDataBuffer();
    CreateInstanceIDBuffer();
    CreateIndirectCommandBuffer();
}

void PrimitiveIndirect::Draw()
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

        indirect_command_.instanceCount = 0;
        glNamedBufferSubData(indirect_buffer_, 0, sizeof(DrawElementsIndirectCommand), &indirect_command_);

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
        glBindBufferBase(GL_UNIFORM_BUFFER, 3, camera_culling_data_buffer_);

        auto &textures = mesh_->GetTextures();
        if (textures.size() > 0)
        {
            glBindTextureUnit(0, textures[0].id);
        }

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_buffer_);

        glBindVertexArray(mesh_->GetVAO());
        glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);
    }
}

void PrimitiveIndirect::CreateObjectDataBuffer()
{
    object_data_.resize(instance_transforms_.size());
    for (size_t i = 0; i < instance_transforms_.size(); i++)
    {
        object_data_[i].model = instance_transforms_[i];
    }
    glCreateBuffers(1, &object_data_buffer_);
    glNamedBufferStorage(object_data_buffer_, object_data_.size() * sizeof(ObjectData), object_data_.data(),
                         GL_DYNAMIC_STORAGE_BIT);
}

void PrimitiveIndirect::CreateCameraCullingDataBuffer()
{
    // camera_culling_data_.view = camera_->GetView();
    // camera_culling_data_.fov = camera_->GetFov();
    // camera_culling_data_.aspect = camera_->GetAspect();
    // camera_culling_data_.znear = camera_->GetNear();
    // camera_culling_data_.zfar = camera_->GetFar();

    glm::vec3 aabb_min = mesh_->GetAABBMin();
    glm::vec3 aabb_max = mesh_->GetAABBMax();
    glm::vec3 center = (aabb_min + aabb_max) * 0.5f;
    glm::vec3 extent = aabb_max - center;
    float radius = glm::length(extent);
    camera_culling_data_.spherebounds = glm::vec4(center, radius);
    camera_culling_data_.drawCount = instance_transforms_.size();

    glCreateBuffers(1, &camera_culling_data_buffer_);
    glNamedBufferStorage(camera_culling_data_buffer_, sizeof(CameraCullingData), &camera_culling_data_,
                         GL_DYNAMIC_STORAGE_BIT);
}

void PrimitiveIndirect::CreateInstanceIDBuffer()
{
    instance_id_data_.resize(instance_transforms_.size());
    for (size_t i = 0; i < instance_transforms_.size(); i++)
    {
        instance_id_data_[i].id = i;
    }
    glCreateBuffers(1, &instance_id_buffer_);
    glNamedBufferStorage(instance_id_buffer_, instance_id_data_.size() * sizeof(InstanceIDData),
                         instance_id_data_.data(), GL_DYNAMIC_STORAGE_BIT);
}

void PrimitiveIndirect::CreateIndirectCommandBuffer()
{
    indirect_command_.count = mesh_->GetIndicesCount();
    indirect_command_.instanceCount = instance_transforms_.size();
    indirect_command_.firstIndex = 0;
    indirect_command_.baseVertex = 0;
    indirect_command_.baseInstance = 0;

    glCreateBuffers(1, &indirect_buffer_);
    glNamedBufferStorage(indirect_buffer_, sizeof(DrawElementsIndirectCommand), &indirect_command_,
                         GL_DYNAMIC_STORAGE_BIT);
}