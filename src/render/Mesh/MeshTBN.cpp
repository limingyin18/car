#include "MeshTBN.hpp"

#include "Vertex.hpp"

#include "glad/glad.h"

using namespace std;

void MeshTBN::Init(const std::vector<VertexNormalMap> &_vertices, const std::vector<uint32_t> &_indices)
{
    vertices_ = _vertices;
    indices_ = _indices;

    SetupBuffer();
    SetupVAO();
}

void MeshTBN::Init(const std::vector<VertexNormalMap> &_vertices, const std::vector<uint32_t> &_indices,
                   const std::vector<Texture> &_textures)
{
    vertices_ = _vertices;
    indices_ = _indices;
    textures_ = _textures;

    SetupBuffer();
    SetupVAO();
}

void MeshTBN::SetupBuffer()
{
    // create vertex buffer
    glCreateBuffers(1, &vbo_);
    glNamedBufferStorage(vbo_, vertices_.size() * sizeof(VertexNormalMap), vertices_.data(), GL_DYNAMIC_STORAGE_BIT);

    // create index buffer
    glCreateBuffers(1, &ebo_);
    glNamedBufferStorage(ebo_, indices_.size() * sizeof(unsigned int), indices_.data(), 0);
    indices_count_ = indices_.size();
}

void MeshTBN::SetupVAO()
{
    if (vao_ != -1)
    {
        return;
    }
    // create vertex array object
    glCreateVertexArrays(1, &vao_);
    glVertexArrayVertexBuffer(vao_, 0, vbo_, 0, sizeof(VertexNormalMap));
    glVertexArrayElementBuffer(vao_, ebo_);

    VertexNormalMap::SetupVAO(vao_);
}

void MeshTBN::UpdateVBO()
{
    glNamedBufferSubData(vbo_, 0, vertices_.size() * sizeof(VertexNormalMap), vertices_.data());
}