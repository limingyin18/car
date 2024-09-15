#include "Material.hpp"

#include "render/Mesh/Vertex.hpp"
#include "render/Shader/Shader.hpp"
#include "spdlog/spdlog.h"

void Material::Use(const std::shared_ptr<Shader> shader)
{
    for (auto &[name, value] : uniform_floats_)
    {
        shader->setFloat(name, value);
    }

    for (auto &[name, value] : uniform_vec3s_)
    {
        shader->setVec3(name, value);
    }

    for (auto &[name, value] : uniform_mat4s_)
    {
        shader->setMat4(name, value);
    }

    for (size_t i = 0; i < textures_.size(); ++i)
    {
        glBindTextureUnit(i, textures_[i].id);
    }
}

void Material::SetFloat(const std::string &name, float value)
{
    uniform_floats_[name] = value;
}

void Material::SetVec3(const std::string &name, const glm::vec3 &value)
{
    uniform_vec3s_[name] = value;
}

void Material::SetMat4(const std::string &name, const glm::mat4 &value)
{
    uniform_mat4s_[name] = value;
}

void Material::AddTexture(const Texture &texture)
{
    textures_.push_back(texture);
}