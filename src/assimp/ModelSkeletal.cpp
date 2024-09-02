#include "ModelSkeletal.hpp"

#include "Bone.hpp"
#include "helper.hpp"
#include "render/Mesh/MeshSkeletal.hpp"
#include "render/Mesh/Vertex.hpp"

std::shared_ptr<IMesh> ModelSkeletal::processMesh(aiMesh *mesh, const aiScene *scene)
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
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
        // textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    }

    ExtractBoneWeightForVertices(vertices, mesh, scene);
    std::shared_ptr<MeshSkeletal> _mesh = std::make_shared<MeshSkeletal>();
    _mesh->Init(vertices, indices, textures);
    _mesh->SetAABBMin(AssimpGLMHelpers::GetGLMVec(mesh->mAABB.mMin));
    _mesh->SetAABBMax(AssimpGLMHelpers::GetGLMVec(mesh->mAABB.mMax));
    return _mesh;
}

void ModelSkeletal::ExtractBoneWeightForVertices(std::vector<VertexSkeletal> &vertices, aiMesh *mesh,
                                                 const aiScene *scene)
{
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_BoneCounter;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_BoneCounter;
            m_BoneCounter++;
        }
        else
        {
            boneID = m_BoneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

void ModelSkeletal::SetVertexBoneData(VertexSkeletal &vertex, int boneID, float weight)
{
    for (int i = 0; i < 4; ++i)
    {
        if (vertex.bone_ids_[i] < 0)
        {
            vertex.bone_weights_[i] = weight;
            vertex.bone_ids_[i] = boneID;
            break;
        }
    }
}