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

    for (auto &[name, value] : textures_)
    {
        glBindTextureUnit(name, textures_[name].id);
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

void Material::SetTexture(uint32_t binding, const Texture &texture)
{
    textures_[binding] = texture;
}