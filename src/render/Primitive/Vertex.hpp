#pragma once

#include <glm/glm.hpp>

#include <string>

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Vertex
{
  public:
    Vertex(glm::vec3 _position = glm::vec3(0.f), glm::vec3 _normal = glm::vec3(0.f, 0.f, 1.f),
           glm::vec2 _uv = glm::vec2(0.f))
        : position_(_position), normal_(_normal), uv_(_uv)
    {
    }

    glm::vec3 position_;
    glm::vec3 normal_;
    glm::vec2 uv_;

    static void SetupVAO(uint32_t _vao);
};

class VertexNormalMap : public Vertex
{
  public:
    VertexNormalMap(glm::vec3 _position = glm::vec3(0.f), glm::vec3 _normal = glm::vec3(0.f, 0.f, 1.f),
                    glm::vec2 _uv = glm::vec2(0.f), glm::vec3 _tangent = glm::vec3(0.f),
                    glm::vec3 _bitangent = glm::vec3(0.f))
        : Vertex(_position, _normal, _uv), tangent_(_tangent), bitangent_(_bitangent)
    {
    }
    glm::vec3 tangent_;
    glm::vec3 bitangent_;

    static void SetupVAO(uint32_t _vao);
};

class VertexSkeletal : public VertexNormalMap
{
  public:
    VertexSkeletal(glm::vec3 _position = glm::vec3(0.f), glm::vec3 _normal = glm::vec3(0.f, 0.f, 1.f),
                   glm::vec2 _uv = glm::vec2(0.f), glm::vec3 _tangent = glm::vec3(0.f),
                   glm::vec3 _bitangent = glm::vec3(0.f), glm::ivec4 _bone_ids = glm::ivec4(-1),
                   glm::vec4 _bone_weights = glm::vec4(0.f))
        : VertexNormalMap(_position, _normal, _uv, _tangent, _bitangent), bone_ids_(_bone_ids),
          bone_weights_(_bone_weights)
    {
    }
    glm::ivec4 bone_ids_;
    glm::vec4 bone_weights_;

    static void SetupVAO(uint32_t _vao);
};