#include "MeshSkeletal.hpp"

#include "Vertex.hpp"

#include "glad/glad.h"

using namespace std;

void MeshSkeletal::Init(const std::vector<VertexSkeletal> &_vertices, const std::vector<uint32_t> &_indices,
                        const std::vector<Texture> &_textures)
{
    vertices_ = _vertices;
    indices_ = _indices;
    textures_ = _textures;

    SetupBuffer();
    SetupVAO();
}

void MeshSkeletal::SetupBuffer()
{
    // create vertex buffer
    glCreateBuffers(1, &vbo_);
    glNamedBufferStorage(vbo_, vertices_.size() * sizeof(VertexSkeletal), vertices_.data(), 0);

    // create index buffer
    glCreateBuffers(1, &ebo_);
    glNamedBufferStorage(ebo_, indices_.size() * sizeof(unsigned int), indices_.data(), 0);
    indices_count_ = indices_.size();
}

void MeshSkeletal::SetupVAO()
{
    if (vao_ != -1)
    {
        return;
    }
    // create vertex array object
    glCreateVertexArrays(1, &vao_);
    glVertexArrayVertexBuffer(vao_, 0, vbo_, 0, sizeof(VertexSkeletal));
    glVertexArrayElementBuffer(vao_, ebo_);

    VertexSkeletal::SetupVAO(vao_);
}