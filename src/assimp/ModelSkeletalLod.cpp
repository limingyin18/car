#include "ModelSkeletalLod.hpp"

#include "Bone.hpp"
#include "assimp/material.h"
#include "helper.hpp"
#include "render/Mesh/MeshSkeletalLod.hpp"
#include "render/Mesh/Vertex.hpp"
#include "spdlog/spdlog.h"

std::shared_ptr<IMesh> ModelSkeletalLod::processMesh(aiMesh *mesh, const aiScene *scene)
{
    using vertex_type = VertexSkeletal;

    std::vector<vertex_type> vertices;
    vertices.reserve(mesh->mNumVertices);
    std::vector<unsigned int> indices;
    indices.reserve(mesh->mNumFaces * 3);
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertex_type vertex;

        // 处理顶点位置、法线和纹理坐标
        ProcessVertex(mesh, i, vertex);

        vertices.push_back(vertex);
    }
    // 处理索引
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // 处理材质
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        // spdlog::debug("path {}", normalMaps[0].path);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
        // textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    }

    ExtractBoneWeightForVertices(vertices, mesh, scene);
    std::shared_ptr<MeshSkeletalLod> _mesh = std::make_shared<MeshSkeletalLod>();
    _mesh->Init(vertices, indices, textures);
    _mesh->SetAABBMin(AssimpGLMHelpers::GetGLMVec(mesh->mAABB.mMin));
    _mesh->SetAABBMax(AssimpGLMHelpers::GetGLMVec(mesh->mAABB.mMax));
    return _mesh;
}

void ModelSkeletalLod::ProcessVertex(aiMesh *mesh, uint32_t index, VertexSkeletal &vertex)
{
    Model::ProcessVertex(mesh, index, vertex);

    // tangent
    if (mesh->mTangents != nullptr)
    {
        vertex.tangent_ = AssimpGLMHelpers::GetGLMVec(mesh->mTangents[index]);
    }

    // bitangent
    if (mesh->mBitangents != nullptr)
    {
        vertex.bitangent_ = AssimpGLMHelpers::GetGLMVec(mesh->mBitangents[index]);
    }
}