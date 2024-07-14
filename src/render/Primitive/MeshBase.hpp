#pragma once

#include "render/Shader/Shader.hpp"

#include <glm/glm.hpp>
#include <vector>

struct VertexBase
{
    glm::vec3 position_;
    glm::vec3 normal_;
    glm::vec3 color_;
};

class MeshBase
{
public:
    MeshBase() = default;
    ~MeshBase() = default;
    void Init(const std::vector<VertexBase> &_vertices,
              const std::vector<uint32_t> &_indices);
    void SetShader(Shader &shader) { shader_ = shader; };
    Shader &GetShader() { return shader_; }
    void Draw();

private:
    void SetupBuffer();
    void SetupVAO();

    std::vector<VertexBase> vertices_;
    std::vector<uint32_t> indices_;
    Shader shader_;
    uint32_t vbo_, ebo_;
    uint32_t vao_;

    using vertex_type = VertexBase;
};