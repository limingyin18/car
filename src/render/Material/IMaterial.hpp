#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>

class Shader;
class Texture;

class IMaterial
{
  public:
    void virtual Use(const std::shared_ptr<Shader> shader) = 0;
    void virtual SetTexture(uint32_t binding, const Texture &texture) = 0;

    void virtual SetMat4(const std::string &name, const glm::mat4 &value) = 0;
};