#include "Primitive.hpp"

#include <spdlog/spdlog.h>

using namespace std;

Primitive::~Primitive()
{
    mesh_ = nullptr;

    spdlog::info("primitive destruct");
}

void Primitive::Draw(std::shared_ptr<Shader> _shader)
{
    _shader->use();
    _shader->setMat4("model", mesh_->GetModel());

    const auto &textures = mesh_->GetTextures();
    if (textures.size() > 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0].id);
    }

    glBindVertexArray(mesh_->GetVAO());
    glDrawElements(GL_TRIANGLES, mesh_->GetIndicesCount(), GL_UNSIGNED_INT, 0);
}