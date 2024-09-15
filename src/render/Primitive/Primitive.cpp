#include "Primitive.hpp"

#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Shader/Shader.hpp"

#include <cstddef>
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
    for (size_t i = 0; i < textures.size(); ++i)
    {
        glBindTextureUnit(i, textures[i].id);
    }

    if (material_ != nullptr)
    {
        material_->Use(shader_);
    }

    glBindVertexArray(mesh_->GetVAO());
    glDrawElements(GL_TRIANGLES, mesh_->GetIndicesCount(), GL_UNSIGNED_INT, 0);
}

void Primitive::Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
                     const std::shared_ptr<IMaterial> &material, glm::mat4 transform)
{
    mesh_ = mesh;
    shader_ = shader;
    material_ = material;
    transform_ = transform;
}
