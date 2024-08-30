#pragma once

#include "IModel.hpp"

#include <assimp/scene.h>

class Vertex;
class Texture;
class aiMesh;
class aiNode;
class aiMaterial;
namespace Assimp
{
class Importer;
};

class Model : public IModel
{
  public:
    Model();
    virtual ~Model();

    void Load(const std::string &_path) override;
    [[nodiscard]] std::vector<std::shared_ptr<IMesh>> &GetMeshes() override
    {
        return meshes_;
    };

    [[nodiscard]] std::string GetDirectory() const
    {
        return directory_;
    };
    [[nodiscard]] const aiScene *GetScene() const override;

  protected:
    virtual std::shared_ptr<IMesh> processMesh(aiMesh *mesh, const aiScene *scene);
    void processNode(aiNode *node, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

    std::unique_ptr<Assimp::Importer> importer;
    std::vector<std::shared_ptr<IMesh>> meshes_;
    std::string directory_;
    void ProcessVertex(aiMesh *mesh, uint32_t index, Vertex &vertex);
};
