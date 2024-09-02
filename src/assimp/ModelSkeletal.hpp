#pragma once

#include "Model.hpp"

#include <glm/glm.hpp>
#include <map>

class VertexSkeletal;
class BoneInfo;

class ModelSkeletal : public Model
{
  public:
    ~ModelSkeletal() = default;
    auto &GetBoneInfoMap()
    {
        return m_BoneInfoMap;
    }
    int &GetBoneCount()
    {
        return m_BoneCounter;
    }

  protected:
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    std::shared_ptr<IMesh> processMesh(aiMesh *mesh, const aiScene *scene) override;

    void ExtractBoneWeightForVertices(std::vector<VertexSkeletal> &vertices, aiMesh *mesh, const aiScene *scene);

    void SetVertexBoneData(VertexSkeletal &vertex, int boneID, float weight);
};