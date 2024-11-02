#pragma once

#include <glm/glm.hpp>

#include <memory>

class IMesh;
class Shader;

class IPrimitive
{
  public:
    virtual ~IPrimitive() = default;
    virtual void Draw() = 0;

    [[nodiscard]] virtual std::shared_ptr<IMesh> GetMesh() const
    {
        return nullptr;
    };
    [[nodiscard]] virtual std::shared_ptr<Shader> GetShader() const
    {
        return nullptr;
    };
    virtual void SetShader(const std::shared_ptr<Shader> &shader) {};

    virtual void SetTransform(const glm::mat4 &transform) {};

    [[nodiscard]] virtual glm::mat4 GetTransform() const
    {
        return {1.0f};
    };
};