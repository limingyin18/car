#pragma once

#include "Shader.hpp"

#include <glm/glm.hpp>
#include <vector>

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

struct Vertex
{
    glm::vec3 position_;
    glm::vec3 normal_;
    glm::vec3 color_;
    glm::vec2 uv_;
};

class Mesh
{
public:
    Mesh() = default;
    ~Mesh();
    void Init(const std::vector<Vertex> &_vertices,
              const std::vector<uint32_t> &_indices);

    void Init(const std::vector<Vertex> &_vertices,
              const std::vector<uint32_t> &_indices,
              const std::vector<Texture> &_textures);
    void SetModel(const glm::mat4 &model) { model_ = model; }
    glm::mat4 &GetModel() { return model_; }
    void SetModelMatrix(const glm::mat4 &model) { model_ = model; }
    void SetName(const std::string &name) { name_ = name; }

    std::vector<Texture> &GetTextures() { return textures_; }

    uint32_t GetVAO() { return vao_; }

    uint32_t GetIndicesCount() { return indices_count_; }

private:
    void SetupBuffer();
    void SetupVAO();

    std::vector<Vertex> vertices_{};
    std::vector<uint32_t> indices_{};
    std::vector<Texture> textures_{};

    glm::mat4 model_{1.0f};
    uint32_t vbo_ = -1, ebo_ = -1;
    uint32_t vao_ = -1;
    uint32_t indices_count_ = -1;

    std::string name_;

    using vertex_type = Vertex;
};