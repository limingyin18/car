#pragma once

#include "IMesh.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Vertex;
class Texture;

class Mesh : public IMesh
{
  public:
    Mesh();
    ~Mesh();

    void Init(const std::vector<Vertex> &_vertices, const std::vector<uint32_t> &_indices);
    void Init(const std::vector<Vertex> &_vertices, const std::vector<uint32_t> &_indices,
              const std::vector<Texture> &_textures);

    void SetModel(const glm::mat4 &model) override
    {
        model_ = model;
    }
    [[nodiscard]] glm::mat4 GetModel() const override
    {
        return model_;
    }

    void SetName(const std::string &name) override
    {
        name_ = name;
    }
    [[nodiscard]] std::string GetName() const override
    {
        return name_;
    }

    void SetAABBMin(const glm::vec3 &aabb_min) override
    {
        aabb_min_ = aabb_min;
    }
    [[nodiscard]] glm::vec3 GetAABBMin() const override
    {
        return aabb_min_;
    }

    void SetAABBMax(const glm::vec3 &aabb_max) override
    {
        aabb_max_ = aabb_max;
    }
    [[nodiscard]] glm::vec3 GetAABBMax() const override
    {
        return aabb_max_;
    }

    [[nodiscard]] std::vector<Vertex> &GetVertices()
    {
        return vertices_;
    }

    [[nodiscard]] std::vector<uint32_t> &GetIndices()
    {
        return indices_;
    }

    [[nodiscard]] std::vector<Texture> &GetTextures() override
    {
        return textures_;
    }
    void SetTextures(const std::vector<Texture> &textures);

    uint32_t GetVAO() override
    {
        return vao_;
    }

    uint32_t GetIndicesCount() override
    {
        return indices_count_;
    }

  protected:
    std::vector<Vertex> vertices_;
    std::vector<uint32_t> indices_;
    std::vector<Texture> textures_;

    glm::vec3 aabb_min_{0.0f};
    glm::vec3 aabb_max_{0.0f};
    glm::mat4 model_{1.0f};

    uint32_t vbo_ = -1, ebo_ = -1;
    uint32_t vao_ = -1;
    uint32_t indices_count_ = -1;

    std::string name_ = "";

    void SetupBuffer() override;
    void SetupVAO() override;
};