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

void MeshTBN::ReComputeNormal()
{
    for (auto &d : vertices_)
        d.normal_ = {0.f, 0.f, 0.f};

    for (unsigned i = 0; i < indices_.size();)
    {
        auto id1 = indices_[i++];
        auto id2 = indices_[i++];
        auto id3 = indices_[i++];

        auto v1 = vertices_[id1].position_;
        auto v2 = vertices_[id2].position_;
        auto v3 = vertices_[id3].position_;

        // This does weighted area based on triangle area
        auto n = glm::cross(v2 - v1, v3 - v1);

        vertices_[id1].normal_ += n;
        vertices_[id2].normal_ += n;
        vertices_[id3].normal_ += n;
    }

    for (auto &d : vertices_)
        d.normal_ = glm::normalize(d.normal_);
}