#include "Model.hpp"
#include "helper.hpp"
#include "render/Primitive/Mesh.hpp"
#include "render/Primitive/Vertex.hpp"
#include "tools/Tool.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <vector>

#include <spdlog/spdlog.h>

using namespace std;

Model::Model()
{
    // spdlog::debug("model destruct");
}

Model::Model::~Model()
{
    // spdlog::debug("model destruct");
}

void Model::Load(const std::string &_path)
{
    // Assimp::Importer importer;
    importer = make_unique<Assimp::Importer>();
    const aiScene *scene = importer->ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::string error = std::string("ERROR::ASSIMP::") + importer->GetErrorString();
        spdlog::error(error);
        throw std::runtime_error(error);
        return;
    }
    directory_ = _path.substr(0, _path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    glm::mat4 model_matrix = AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);
    aiNode *parent = node->mParent;
    while (parent != nullptr)
    {
        glm::mat4 model_matrix_parent = AssimpGLMHelpers::ConvertMatrixToGLMFormat(parent->mTransformation);
        model_matrix = model_matrix_parent * model_matrix;
        parent = parent->mParent;
    }

    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(processMesh(mesh, scene));

        meshes_.back()->SetModel(model_matrix);
        meshes_.back()->SetName(node->mName.C_Str());
    }

    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<IMesh> Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    std::vector<unsigned int> indices;
    indices.reserve(mesh->mNumFaces * 3);
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

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

    std::shared_ptr<Mesh> _mesh = std::make_shared<Mesh>();
    _mesh->Init(vertices, indices, textures);
    return _mesh;
}
void Model::ProcessVertex(aiMesh *mesh, uint32_t index, Vertex &vertex)
{
    // 顶点位置
    vertex.position_ = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[index]);

    // 顶点法线
    vertex.normal_ = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[index]);

    // 纹理坐标
    if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
    {
        vertex.uv_ = {mesh->mTextureCoords[0][index].x, mesh->mTextureCoords[0][index].y};
    }
    else
    {
        vertex.uv_ = glm::vec2(0.0f, 0.0f);
    }
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        std::string path = directory_ + '/' + str.C_Str();
        texture.id = Tool::LoadTexture(path);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }

    return textures;
}

const aiScene *Model::GetScene() const
{
    return importer->GetScene();
}