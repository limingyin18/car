#include "MeshBase.hpp"

#include "glad/glad.h"

void MeshBase::Init(const std::vector<VertexBase> &_vertices,
                    const std::vector<uint32_t> &_indices)
{
    vertices_ = _vertices;
    indices_ = _indices;

    SetupBuffer();
    SetupVAO();
}

void MeshBase::Draw()
{
    shader_.use();
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
    glUseProgram(0);
    glBindVertexArray(0);
}

void MeshBase::SetupBuffer()
{
    // create vertex buffer
    glCreateBuffers(1, &vbo_);
    glNamedBufferStorage(vbo_, vertices_.size() * sizeof(vertex_type), vertices_.data(), 0);

    // create index buffer
    glCreateBuffers(1, &ebo_);
    glNamedBufferStorage(ebo_, indices_.size() * sizeof(unsigned int), indices_.data(), 0);
}
void MeshBase::SetupVAO()
{
    // create vertex array object
    glCreateVertexArrays(1, &vao_);
    glVertexArrayVertexBuffer(vao_, 0, vbo_, 0, sizeof(vertex_type));
    glVertexArrayElementBuffer(vao_, ebo_);

    uint32_t vertex_attrib_index = 0;
    int vertex_attrib_count = 0;

    // position
    vertex_attrib_count = sizeof(vertex_type::position_) / sizeof(float);
    glEnableVertexArrayAttrib(vao_, vertex_attrib_index);
    glVertexArrayAttribFormat(vao_, vertex_attrib_index, vertex_attrib_count, GL_FLOAT, GL_FALSE,
                              offsetof(vertex_type, position_));
    glVertexArrayAttribBinding(vao_, vertex_attrib_index++, 0);

    // normal
    vertex_attrib_count = sizeof(vertex_type::normal_) / sizeof(float);
    glEnableVertexArrayAttrib(vao_, vertex_attrib_index);
    glVertexArrayAttribFormat(vao_, vertex_attrib_index, vertex_attrib_count, GL_FLOAT, GL_FALSE,
                              offsetof(vertex_type, normal_));
    glVertexArrayAttribBinding(vao_, vertex_attrib_index++, 0);

    // color
    vertex_attrib_count = sizeof(vertex_type::color_) / sizeof(float);
    glEnableVertexArrayAttrib(vao_, vertex_attrib_index);
    glVertexArrayAttribFormat(vao_, vertex_attrib_index, vertex_attrib_count, GL_FLOAT, GL_FALSE,
                              offsetof(vertex_type, color_));
    glVertexArrayAttribBinding(vao_, vertex_attrib_index++, 0);
}