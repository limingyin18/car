#include "Vertex.hpp"

#include "glad/glad.h"
#include "spdlog/spdlog.h"

void Vertex::SetupVAO(uint32_t _vao)
{
    uint32_t vertex_attrib_index = 0;
    int vertex_attrib_count = 0;

    // position
    vertex_attrib_count = sizeof(Vertex::position_) / sizeof(float);
    glEnableVertexArrayAttrib(_vao, vertex_attrib_index);
    glVertexArrayAttribFormat(_vao, vertex_attrib_index, vertex_attrib_count, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, position_));
    glVertexArrayAttribBinding(_vao, vertex_attrib_index++, 0);

    // normal
    vertex_attrib_count = sizeof(Vertex::normal_) / sizeof(float);
    glEnableVertexArrayAttrib(_vao, vertex_attrib_index);
    glVertexArrayAttribFormat(_vao, vertex_attrib_index, vertex_attrib_count, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, normal_));
    glVertexArrayAttribBinding(_vao, vertex_attrib_index++, 0);

    // uv
    vertex_attrib_count = sizeof(Vertex::uv_) / sizeof(float);
    glEnableVertexArrayAttrib(_vao, vertex_attrib_index);
    glVertexArrayAttribFormat(_vao, vertex_attrib_index, vertex_attrib_count, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, uv_));
    glVertexArrayAttribBinding(_vao, vertex_attrib_index++, 0);
}

void VertexNormalMap::SetupVAO(uint32_t _vao)
{
    Vertex::SetupVAO(_vao);

    uint32_t vertex_attrib_index = 3;
    int vertex_attrib_count = 0;

    // tangent
    vertex_attrib_count = sizeof(VertexNormalMap::tangent_) / sizeof(float);
    glEnableVertexArrayAttrib(_vao, vertex_attrib_index);
    glVertexArrayAttribFormat(_vao, vertex_attrib_index, vertex_attrib_count, GL_FLOAT, GL_FALSE,
                              offsetof(VertexNormalMap, tangent_));
    glVertexArrayAttribBinding(_vao, vertex_attrib_index++, 0);

    // bitangent
    vertex_attrib_count = sizeof(VertexNormalMap::bitangent_) / sizeof(float);
    glEnableVertexArrayAttrib(_vao, vertex_attrib_index);
    glVertexArrayAttribFormat(_vao, vertex_attrib_index, vertex_attrib_count, GL_FLOAT, GL_FALSE,
                              offsetof(VertexNormalMap, bitangent_));
    glVertexArrayAttribBinding(_vao, vertex_attrib_index++, 0);
}

void VertexSkeletal::SetupVAO(uint32_t _vao)
{
    VertexNormalMap::SetupVAO(_vao);

    uint32_t vertex_attrib_index = 5;
    int vertex_attrib_count = 0;
    // bone_ids
    vertex_attrib_count = sizeof(VertexSkeletal::bone_ids_) / sizeof(int);
    glEnableVertexArrayAttrib(_vao, vertex_attrib_index);
    glVertexArrayAttribIFormat(_vao, vertex_attrib_index, vertex_attrib_count, GL_INT,
                              offsetof(VertexSkeletal, bone_ids_));
    glVertexArrayAttribBinding(_vao, vertex_attrib_index++, 0);

    // bone_weights
    vertex_attrib_count = sizeof(VertexSkeletal::bone_weights_) / sizeof(float);
    glEnableVertexArrayAttrib(_vao, vertex_attrib_index);
    glVertexArrayAttribFormat(_vao, vertex_attrib_index, vertex_attrib_count, GL_FLOAT, GL_FALSE,
                              offsetof(VertexSkeletal, bone_weights_));
    glVertexArrayAttribBinding(_vao, vertex_attrib_index++, 0);
}