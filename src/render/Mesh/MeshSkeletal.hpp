#pragma once

#include "Mesh.hpp"

class VertexSkeletal;

class MeshSkeletal : public Mesh
{
  public:
    MeshSkeletal() = default;
    ~MeshSkeletal() = default;

    virtual void Init(const std::vector<VertexSkeletal> &_vertices, const std::vector<uint32_t> &_indices,
              const std::vector<Texture> &_textures);

  protected:
    void SetupBuffer() override;
    void SetupVAO() override;

    std::vector<VertexSkeletal> vertices_{};
};