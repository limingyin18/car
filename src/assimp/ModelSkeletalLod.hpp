#pragma once

#include "ModelSkeletal.hpp"

class ModelSkeletalLod : public ModelSkeletal
{
  protected:
    std::shared_ptr<IMesh> processMesh(aiMesh *mesh, const aiScene *scene) override;
    void ProcessVertex(aiMesh *mesh, uint32_t index, VertexSkeletal &vertex);
};