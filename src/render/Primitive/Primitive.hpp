#pragma once

#include <glm/glm.hpp>

#include <spdlog/spdlog.h>

#include <memory>

class IMesh;
class Shader;

class Primitive
{
  public:
    Primitive();
    virtual ~Primitive();

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

    void Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
              glm::mat4 transform = glm::mat4(1.0f));

    virtual void Draw();

  protected:
    std::shared_ptr<IMesh> mesh_ = nullptr;
    std::shared_ptr<Shader> shader_ = nullptr;
    glm::mat4 transform_ = glm::mat4(1.0f);
};