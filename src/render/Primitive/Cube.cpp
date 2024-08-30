#include "Cube.hpp"
#include "Vertex.hpp"

using namespace std;

void Cube::Init(uint32_t _texture)
{
    uint32_t n = 2;

    std::vector<Vertex> vertices;
    vertices.reserve(6 * n * n);
    std::vector<uint32_t> indices;
    indices.reserve(6 * 6 * (n - 1) * (n - 1));

    const float dx = 2.0f / (n - 1);

    // top
    for (int i = 0; i < n; ++i)
    {
        for (int j = n - 1; j >= 0; --j)
        {
            vertices.emplace_back(glm::vec3{-1.0f + i * dx, 1.0f, -1.0f + j * dx}, glm::vec3{0.0f, 1.0f, 0.0f},
                                  glm::vec2{1.0f - static_cast<float>(i) / static_cast<float>(n - 1),
                                            1.0f - static_cast<float>(j) / static_cast<float>(n - 1)});
        }
    }

    // bottom
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            vertices.emplace_back(glm::vec3{-1.0f + i * dx, -1.0f, -1.0f + j * dx}, glm::vec3{0.0f, -1.0f, 0.0f},
                                  glm::vec2{1.0f - static_cast<float>(i) / static_cast<float>(n - 1),
                                            static_cast<float>(j) / static_cast<float>(n - 1)});
        }
    }

    // back
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            vertices.emplace_back(glm::vec3{-1.0f + i * dx, -1.0f + j * dx, -1.0f}, glm::vec3{0.0f, 0.0f, -1.0f},
                                  glm::vec2{static_cast<float>(i) / static_cast<float>(n - 1),
                                            1.0f - static_cast<float>(j) / static_cast<float>(n - 1)});
        }
    }

    // front
    for (int i = 0; i < n; ++i)
    {
        for (int j = n - 1; j >= 0; --j)
        {
            vertices.emplace_back(glm::vec3{-1.0f + i * dx, -1.0f + j * dx, 1.0f}, glm::vec3{0.0f, 0.0f, 1.0f},
                                  glm::vec2{static_cast<float>(i) / static_cast<float>(n - 1),
                                            static_cast<float>(j) / static_cast<float>(n - 1)});
        }
    }

    // left
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            vertices.emplace_back(glm::vec3{-1.0f, -1.0f + i * dx, -1.0f + j * dx}, glm::vec3{-1.0f, 0.0f, 0.0f},
                                  glm::vec2{1.0f - static_cast<float>(j) / static_cast<float>(n - 1),
                                            static_cast<float>(i) / static_cast<float>(n - 1)});
        }
    }

    // right
    for (int i = 0; i < n; ++i)
    {
        for (int j = n - 1; j >= 0; --j)
        {
            vertices.emplace_back(glm::vec3{1.0f, -1.0f + i * dx, -1.0f + j * dx}, glm::vec3{1.0f, 0.0f, 0.0f},
                                  glm::vec2{static_cast<float>(j) / static_cast<float>(n - 1),
                                            static_cast<float>(i) / static_cast<float>(n - 1)});
        }
    }

    // indices
    for (int k = 0; k < 6; ++k)
    {
        for (int i = 0; i < n - 1; ++i)
        {
            for (int j = 0; j < n - 1; ++j)
            {
                const unsigned I = i * n + j + k * n * n;
                indices.emplace_back(I);
                indices.emplace_back(I + n);
                indices.emplace_back(I + 1);

                indices.emplace_back(I + n);
                indices.emplace_back(I + n + 1);
                indices.emplace_back(I + 1);
            }
        }
    }

    // make sure every face vertex order, normal point to out side
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

    Texture texture;
    texture.id = _texture;
    texture.type = "texture_diffuse";
    texture.path = "";
    std::vector<Texture> textures;
    textures.push_back(texture);
    Mesh::Init(vertices, indices, textures);
}