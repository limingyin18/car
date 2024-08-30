#pragma once

#include <memory>
#include <string>
#include <vector>

class IMesh;
class aiScene;

class IModel
{
  public:
    IModel() = default;
    virtual ~IModel() = default;

    [[nodiscard]] virtual std::vector<std::shared_ptr<IMesh>> &GetMeshes() = 0;
    [[nodiscard]] virtual const aiScene *GetScene() const = 0;

    virtual void Load(const std::string &_path) = 0;
};