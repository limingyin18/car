#pragma once

#include "IActor.hpp"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

class IModel;
class IMesh;
class IMaterial;
class Shader;
class Primitive;

class Actor : public IActor
{
  public:
    Actor();
    virtual ~Actor();
    void Init() override;
    void Update() override {};
    void Destroy() override {};

    void SetModelPath(const std::string &model_path)
    {
        model_path_ = model_path;
    };
    [[nodiscard]] std::string GetModelPath() const
    {
        return model_path_;
    };
    void SetShader(const std::shared_ptr<Shader> &shader)
    {
        shader_ = shader;
    };
    [[nodiscard]] std::shared_ptr<Shader> GetShader() const
    {
        return shader_;
    };
    void SetTransform(const glm::mat4 &transform)
    {
        transform_ = transform;
    };
    [[nodiscard]] glm::mat4 GetTransform() const
    {
        return transform_;
    };
    void SetMaterial(const std::shared_ptr<IMaterial> &material)
    {
        material_ = material;
    };
    [[nodiscard]] std::shared_ptr<IMaterial> GetMaterial() const
    {
        return material_;
    };

    std::vector<std::shared_ptr<IPrimitive>> &GetPrimitives() override
    {
        return primitives_;
    }

    void AddMesh(const std::shared_ptr<IMesh> &mesh)
    {
        meshes_.push_back(mesh);
    }
    void SetMeshes(const std::vector<std::shared_ptr<IMesh>> &meshes)
    {
        meshes_ = meshes;
    }
    [[nodiscard]] std::vector<std::shared_ptr<IMesh>> GetMeshes() const
    {
        return meshes_;
    }

  protected:
    virtual void CreatePrimitives();

    std::string model_path_ = "";
    std::shared_ptr<IModel> model_ = nullptr;

    std::vector<std::shared_ptr<IMesh>> meshes_;
    std::shared_ptr<Shader> shader_ = nullptr;
    std::shared_ptr<IMaterial> material_ = nullptr;

    glm::mat4 transform_{1.0f};

    std::vector<std::shared_ptr<IPrimitive>> primitives_;
};