#include "Model.hpp"
#include "helper.hpp"
#include "tools/Tool.hpp"

#include <vector>

#include <spdlog/spdlog.h>

using namespace std;

Model::~Model()
{
    for (auto &mesh : meshes)
    {
        mesh = nullptr;
    }
    meshes.clear();
    spdlog::info("model destruct");
}

void Model::Load(const std::string &path)
{
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

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
        meshes.push_back(processMesh(mesh, scene));

        meshes.back()->SetModel(model_matrix);
        meshes.back()->SetName(node->mName.C_Str());
    }

    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        // 处理顶点位置、法线和纹理坐标
        glm::vec3 pos;
        pos.x = mesh->mVertices[i].x;
        pos.y = mesh->mVertices[i].y;
        pos.z = mesh->mVertices[i].z;
        vertex.position_ = pos;

        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertex.normal_ = normal;

        if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.uv_ = vec;
        }
        else
        {
            vertex.uv_ = glm::vec2(0.0f, 0.0f);
        }

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
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");

        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
        // textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    }

    std::shared_ptr<Mesh> _mesh = std::make_shared<Mesh>();
    _mesh->Init(vertices, indices, textures);
    return _mesh;
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        string path = directory + '/' + str.C_Str();
        texture.id = Tool::LoadTexture(path);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }

    return textures;
}