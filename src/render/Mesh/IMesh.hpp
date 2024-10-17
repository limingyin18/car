#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Texture;

class IMesh
{
  public:
    IMesh() = default;
    virtual ~IMesh() = default;

    virtual void SetModel(const glm::mat4 &model) = 0;
    [[nodiscard]] virtual glm::mat4 GetModel() const = 0;

    virtual void SetName(const std::string &name) = 0;
    [[nodiscard]] virtual std::string GetName() const = 0;

    virtual void SetAABBMin(const glm::vec3 &aabb_min) = 0;
    [[nodiscard]] virtual glm::vec3 GetAABBMin() const = 0;
    virtual void SetAABBMax(const glm::vec3 &aabb_max) = 0;
    [[nodiscard]] virtual glm::vec3 GetAABBMax() const = 0;

    [[nodiscard]] virtual uint32_t GetVAO() = 0;
    [[nodiscard]] virtual uint32_t GetIndicesCount() = 0;
    [[nodiscard]] virtual std::vector<Texture> &GetTextures() = 0;

    virtual void SetupBuffer() = 0;
    virtual void SetupVAO() = 0;

    virtual void ReComputeNormal() = 0;
};