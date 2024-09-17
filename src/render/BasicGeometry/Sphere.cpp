#include "Sphere.hpp"

#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "Tool.hpp"

#include <cmath>
#include <numbers>

using namespace std;

void Sphere::Init(uint32_t lat, uint32_t lng, float radius)
{
    radius_ = radius;

    std::vector<VertexNormalMap> vertices;
    vertices.reserve((lat + 1) * (lng + 1));
    for (uint32_t i = 0; i <= lat; ++i)
    {
        float angLat = numbers::pi / 2.f - i * numbers::pi / lat; // starting from pi/2 to -pi/2
        float xy = radius_ * cosf(angLat);                        // r * cos(u)
        float z = radius_ * sinf(angLat);                         // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= lng; ++j)
        {
            float angLng = j * 2 * numbers::pi / lng; // starting from 0 to 2pi

            // vertex position (x, y, z)
            float x = xy * cosf(angLng); // r * cos(u) * cos(v)
            float y = xy * sinf(angLng); // r * cos(u) * sin(v)

            // normalized vertex normal (nx, ny, nz)
            float nx = x / radius_;
            float ny = y / radius_;
            float nz = z / radius_;

            // vertex tex coord (s, t) range between [0, 1]
            float s = (float)j / lng;
            float t = (float)i / lat;

            vertices.emplace_back(glm::vec3{x, y, z}, glm::vec3{nx, ny, nz}, glm::vec2{s, t});
        }
    }

    // generate CCW index list of sphere triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    std::vector<uint32_t> indices;
    indices.reserve((lat - 1) * lng * 6);
    for (uint32_t i = 0; i < lat; ++i)
    {
        uint32_t k1 = i * (lng + 1); // beginning of current stack
        uint32_t k2 = k1 + lng + 1;  // beginning of next stack

        for (int j = 0; j < lng; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (lat - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    graphics::CalculateTangent(vertices, indices);
    MeshTBN::Init(vertices, indices);
}