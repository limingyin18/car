#include "PrimitiveInstance.hpp"

#include "Primitive.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Shader/Shader.hpp"


void PrimitiveInstance::Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
                             const std::vector<glm::mat4> &instance_transforms)
{
    Primitive::Init(mesh, shader);
    instance_transforms_ = instance_transforms;

    glCreateBuffers(1, &instance_transforms_buffer_);
    glNamedBufferStorage(instance_transforms_buffer_, instance_transforms_.size() * sizeof(glm::mat4),
                         instance_transforms_.data(), GL_DYNAMIC_STORAGE_BIT);
}

void PrimitiveInstance::Draw()
{
    if (mesh_ == nullptr || shader_ == nullptr)
    {
        spdlog::error("Primitive::Draw: mesh or shader is nullptr");
        return;
    }

    shader_->use();
    shader_->setMat4("model", mesh_->GetModel());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instance_transforms_buffer_);

    auto &textures = mesh_->GetTextures();
    if (textures.size() > 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0].id);
    }

    glBindVertexArray(mesh_->GetVAO());
    glDrawElementsInstanced(GL_TRIANGLES, mesh_->GetIndicesCount(), GL_UNSIGNED_INT, 0, instance_transforms_.size());
}