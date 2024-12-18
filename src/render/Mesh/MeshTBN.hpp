#pragma once

#include "Mesh.hpp"

class VertexNormalMap;

class MeshTBN : public Mesh
{
  public:
    MeshTBN() = default;
    ~MeshTBN() = default;

    void Init(const std::vector<VertexNormalMap> &_vertices, const std::vector<uint32_t> &_indices);
    void Init(const std::vector<VertexNormalMap> &_vertices, const std::vector<uint32_t> &_indices,
              const std::vector<Texture> &_textures);

    [[nodiscard]] std::vector<VertexNormalMap> &GetVertices()
    {
        return vertices_;
    }

    void UpdateVBO() override;

    void ReComputeNormal() override;

  protected:
    void SetupBuffer() override;
    void SetupVAO() override;

    std::vector<VertexNormalMap> vertices_;
};