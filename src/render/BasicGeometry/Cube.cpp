#include "Cube.hpp"
#include "render/Mesh/Vertex.hpp"

using namespace std;

void Cube::Init(size_t n)
{
    assert(n >= 2);

    std::vector<Vertex> vertices;
    vertices.reserve(6 * n * n);
    std::vector<uint32_t> indices;
    indices.reserve(6 * (n - 1) * 6 * (n - 1));

    const float dx = 1.0f / static_cast<float>(n - 1);
    const float dz = 1.0f / static_cast<float>(n - 1);
    const float x0 = -0.5f;
    const float z0 = -0.5f;

    // top
    {
        const glm::vec3 normal = glm::vec3{0.0f, 1.0f, 0.0f};
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                float x = static_cast<float>(i) * dx;
                float z = static_cast<float>(j) * dz;
                glm::vec3 pos = glm::vec3{x0 + x, 0.5f, z0 + z};
                glm::vec2 uv = glm::vec2{x, z};
                vertices.emplace_back(pos, normal, uv);
            }
        }
    }

    // bottom
    {
        const glm::vec3 normal = glm::vec3{0.0f, -1.0f, 0.0f};
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                float x = static_cast<float>(i) * dx;
                float z = static_cast<float>(n - 1 - j) * dz;
                glm::vec3 pos = glm::vec3{x0 + x, -0.5f, z0 + z};
                glm::vec2 uv = glm::vec2{x, 1.0f - z};
                vertices.emplace_back(pos, normal, uv);
            }
        }
    }

    // back
    {
        const glm::vec3 normal = glm::vec3{0.0f, 0.0f, -1.0f};
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                float x = static_cast<float>(i) * dx;
                float z = static_cast<float>(j) * dz;
                glm::vec3 pos = glm::vec3{x0 + x, z0 + z, -0.5f};
                glm::vec2 uv = glm::vec2{1.0f - x, 1.0f - z};
                vertices.emplace_back(pos, normal, uv);
            }
        }
    }

    // front
    {
        const glm::vec3 normal = glm::vec3{0.0f, 0.0f, 1.0f};
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                float x = static_cast<float>(i) * dx;
                float z = static_cast<float>(n - 1 - j) * dz;
                glm::vec3 pos = glm::vec3{x0 + x, z0 + z, 0.5f};
                glm::vec2 uv = glm::vec2{x, 1.0f - z};
                vertices.emplace_back(pos, normal, uv);
            }
        }
    }

    // left
    {
        const glm::vec3 normal = glm::vec3{-1.0f, 0.0f, 0.0f};
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                float x = static_cast<float>(i) * dx;
                float z = static_cast<float>(j) * dz;
                glm::vec3 pos = glm::vec3{-0.5f, x0 + x, z0 + z};
                glm::vec2 uv = glm::vec2{z, 1.0f - x};
                vertices.emplace_back(pos, normal, uv);
            }
        }
    }

    // right
    {
        const glm::vec3 normal = glm::vec3{1.0f, 0.0f, 0.0f};
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                float x = static_cast<float>(i) * dx;
                float z = static_cast<float>(n - 1 - j) * dz;
                glm::vec3 pos = glm::vec3{0.5f, x0 + x, z0 + z};
                glm::vec2 uv = glm::vec2{1.0f - z, 1.0f - x};
                vertices.emplace_back(pos, normal, uv);
            }
        }
    }

    // indices
    for (int k = 0; k < 6; ++k)
    {
        for (int i = 0; i < n - 1; ++i)
        {
            for (int j = 0; j < n - 1; ++j)
            {
                unsigned I = i * n + j + k * n * n;
                indices.emplace_back(I);
                indices.emplace_back(I + 1);
                indices.emplace_back(I + n);

                indices.emplace_back(I + n);
                indices.emplace_back(I + 1);
                indices.emplace_back(I + n + 1);
            }
        }
    }

    Mesh::Init(vertices, indices);
}