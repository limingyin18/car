#pragma once

#include "IMaterial.hpp"

#include <cstdint>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

class Texture;

class Material : public IMaterial
{
  protected:
    std::unordered_map<std::string, float> uniform_floats_{};
    std::unordered_map<std::string, glm::vec3> uniform_vec3s_{};
    std::unordered_map<std::string, glm::mat4> uniform_mat4s_{};
    std::unordered_map<uint32_t, Texture> textures_{};

  public:
    void Use(const std::shared_ptr<Shader> shader) override;
    void SetFloat(const std::string &name, float value);
    void SetVec3(const std::string &name, const glm::vec3 &value);
    void SetMat4(const std::string &name, const glm::mat4 &value) override;
    void SetTexture(uint32_t binding, const Texture &texture) override;
};
