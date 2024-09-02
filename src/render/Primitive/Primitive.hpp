#pragma once

#include "IPrimitive.hpp"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <memory>

class IMesh;
class Shader;

class Primitive : public IPrimitive
{
  protected:
    std::shared_ptr<IMesh> mesh_ = nullptr;
    std::shared_ptr<Shader> shader_ = nullptr;
    glm::mat4 transform_ = glm::mat4(1.0f);

  public:
    void Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
              glm::mat4 transform = glm::mat4(1.0f));

    void Draw() override;

    void SetTransform(const glm::mat4 &transform)
    {
        transform_ = transform;
    }

    [[nodiscard]] glm::mat4 GetTransform() const
    {
        return transform_;
    }

    void SetShader(const std::shared_ptr<Shader> &shader)
    {
        shader_ = shader;
    }

    [[nodiscard]] std::shared_ptr<Shader> GetShader() const
    {
        return shader_;
    }

    void SetMesh(std::shared_ptr<IMesh> &mesh)
    {
        mesh_ = mesh;
    }

    [[nodiscard]] std::shared_ptr<IMesh> GetMesh() const
    {
        return mesh_;
    }
};