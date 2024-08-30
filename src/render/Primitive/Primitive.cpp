#include "Primitive.hpp"

#include "../Shader/Shader.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include <memory>

Primitive::Primitive() {
    // spdlog::debug("Primitive construct");
};
Primitive::~Primitive() {
    // spdlog::debug("Primitive destruct");
};

void Primitive::Draw()
{
    if (mesh_ == nullptr || shader_ == nullptr)
    {
        spdlog::error("Primitive::Draw: mesh or shader is nullptr");
        return;
    }

    shader_->use();
    shader_->setMat4("model", transform_ * mesh_->GetModel());

    auto &textures = mesh_->GetTextures();
    if (textures.size() > 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0].id);
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
