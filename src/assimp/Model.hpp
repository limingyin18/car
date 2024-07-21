#pragma once

#include "render/Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Model
{
public:
    Model() = default;
    ~Model();
    void Load(const std::string &path);
    std::vector<std::shared_ptr<Mesh>> &GetMeshes() { return meshes; };

private:
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::string directory;
    void processNode(aiNode *node, const aiScene *scene);
    virtual std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};