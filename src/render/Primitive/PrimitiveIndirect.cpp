#include "PrimitiveIndirect.hpp"

#include "../Shader/Shader.hpp"

#include "Mesh.hpp"
#include "Vertex.hpp"

void PrimitiveIndiect::Draw()
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