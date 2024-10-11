#include "Mesh.hpp"

#include "Vertex.hpp"

#include "glad/glad.h"

#include <spdlog/spdlog.h>

Mesh::Mesh() = default;

Mesh::~Mesh()
{
    // spdlog::debug("Mesh {} deleted", name_);

    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
    glDeleteVertexArrays(1, &vao_);
}

void Mesh::Init(const std::vector<Vertex> &_vertices, const std::vector<uint32_t> &_indices)
{
    vertices_ = _vertices;
    indices_ = _indices;

    SetupBuffer();
    SetupVAO();
}

void Mesh::Init(const std::vector<Vertex> &_vertices, const std::vector<uint32_t> &_indices,
                const std::vector<Texture> &_textures)
{
    Init(_vertices, _indices);
    textures_ = _textures;
}

void Mesh::SetupBuffer()
{
    // create vertex buffer
    glCreateBuffers(1, &vbo_);
    glNamedBufferStorage(vbo_, vertices_.size() * sizeof(Vertex), vertices_.data(), vbo_flag_);

    // create index buffer
    glCreateBuffers(1, &ebo_);
    glNamedBufferStorage(ebo_, indices_.size() * sizeof(unsigned int), indices_.data(), 0);
    indices_count_ = indices_.size();
}

void Mesh::SetupVAO()
{
    if (vao_ != -1)
    {
        return;
    }
    // create vertex array object
    glCreateVertexArrays(1, &vao_);
    glVertexArrayVertexBuffer(vao_, 0, vbo_, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(vao_, ebo_);

    Vertex::SetupVAO(vao_);
}

void Mesh::SetTextures(const std::vector<Texture> &textures)
{
    textures_ = textures;
}

void Mesh::UpdateVertices()
{
    glNamedBufferSubData(vbo_, 0, vertices_.size() * sizeof(Vertex), vertices_.data());
}

void Mesh::SetDynamic()
{
    vbo_flag_ = GL_DYNAMIC_STORAGE_BIT;
}