#include "Tool.hpp"

#include "render/Mesh/Vertex.hpp"
#include <algorithm>
#include <glm/glm.hpp>

void graphics::Order(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
{
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        auto v1 = vertices[indices[i]].position_;
        auto v2 = vertices[indices[i + 1]].position_;
        auto v3 = vertices[indices[i + 2]].position_;
        float volume_tetrahedron =
            glm::dot(v1, glm::cross(v2, v3)); // Needs to be divided by 6, postpone this until the end of the loop
        if (volume_tetrahedron < 0.f)
        {
            std::swap(indices[i + 1], indices[i + 2]);
        }
    }
}

void graphics::CalculateTangent(std::vector<VertexNormalMap> &vertices, const std::vector<uint32_t> &indices)
{
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        auto &v1 = vertices[indices[i]];
        auto &v2 = vertices[indices[i + 1]];
        auto &v3 = vertices[indices[i + 2]];

        glm::vec3 edge1 = v2.position_ - v1.position_;
        glm::vec3 edge2 = v3.position_ - v1.position_;
        glm::vec2 deltaUV1 = v2.uv_ - v1.uv_;
        glm::vec2 deltaUV2 = v3.uv_ - v1.uv_;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        v1.tangent_ += tangent;
        v2.tangent_ += tangent;
        v3.tangent_ += tangent;

        v1.bitangent_ = glm::cross(v1.normal_, v1.tangent_);
        v2.bitangent_ = glm::cross(v2.normal_, v2.tangent_);
        v3.bitangent_ = glm::cross(v3.normal_, v3.tangent_);
    }

    for (auto &v : vertices)
    {
        v.tangent_ = glm::normalize(v.tangent_);
        v.bitangent_ = glm::normalize(v.bitangent_);
    }
}