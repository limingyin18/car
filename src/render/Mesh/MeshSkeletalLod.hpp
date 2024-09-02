#pragma once

#include "MeshSkeletal.hpp"

class MeshSkeletalLod : public MeshSkeletal
{
  public:
    void GenerateLOD();

    void Init(const std::vector<VertexSkeletal> &_vertices, const std::vector<uint32_t> &_indices,
              const std::vector<Texture> &_textures) override;

    [[nodiscard]] const std::vector<uint32_t> &GetLods() const
    {
        return lods_;
    }
  protected:
    std::vector<uint32_t> lods_;
};