#include "Primitive.hpp"

#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Shader/Shader.hpp"

#include <memory>

void Primitive::Draw()
{
    if (mesh_ == nullptr || shader_ == nullptr)
    {
        throw std::runtime_error("Primitive::Draw: mesh or shader is nullptr");
    }

    shader_->use();
    shader_->setMat4("model", transform_ * mesh_->GetModel());

    auto &textures = mesh_->GetTextures();
    if (textures.size() > 0)
    {
        glBindTextureUnit(0, textures[0].id);
    }

    glBindVertexArray(mesh_->GetVAO());
    glDrawElements(GL_TRIANGLES, mesh_->GetIndicesCount(), GL_UNSIGNED_INT, 0);
}

void Primitive::Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader, glm::mat4 transform)
{
    mesh_ = mesh;
    shader_ = shader;
    transform_ = transform;
}
