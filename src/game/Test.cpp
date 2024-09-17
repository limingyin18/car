#include "Game.hpp"

#include "Actor/Actor.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"
#include "render/Shader/Shader.hpp"
#include "spdlog/spdlog.h"
#include <cstddef>

using namespace std;

void Game::TestBasicGeometry()
{
    spdlog::info("TestBasicGeometry");

    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    if (false)
    {
        auto cube = std::make_shared<Actor>();
        cube->AddMesh(meshes_map["cube"]);
        cube->SetShader(shaders_map["equirectangular2cubemap"]);
        auto &textures = meshes_map["cube"]->GetTextures();
        textures[0].id = render.GetTexture("assets/textures/hdr/newport_loft.hdr");
        actors_.push_back(cube);
    }

    if (false)
    {
        auto &textures = meshes_map["sphere"]->GetTextures();
        textures[0].id = render.GetTexture("assets/textures/sphere/rustediron2_basecolor.png");
        // textures.push_back(texture);
        Texture texture;
        texture.id = render.GetTexture("assets/textures/sphere/rustediron2_normal.png");
        textures.push_back(texture);
        texture.id = render.GetTexture("assets/textures/sphere/rustediron2_metallic.png");
        textures.push_back(texture);
        texture.id = render.GetTexture("assets/textures/sphere/rustediron2_roughness.png");
        textures.push_back(texture);
        for (size_t i = 0; i < 10; i++)
        {
            for (size_t j = 0; j < 10; j++)
            {
                float x = static_cast<float>(i);
                float y = static_cast<float>(j);

                auto sphere = std::make_shared<Actor>();
                sphere->AddMesh(meshes_map["sphere"]);
                sphere->SetShader(shaders_map["pbr_uniform"]);
                auto material = std::make_shared<Material>();
                material->SetVec3("albedo", glm::vec3{1.0f, 0.0f, 0.0f});
                material->SetFloat("metallic", x / 10.0f);
                material->SetFloat("roughness", y / 10.0f);
                material->SetFloat("ao", 1.0f);

                Texture texture;
                texture.id = render.GetConvolutionCubemap();
                material->SetTexture(4, texture);
                texture.id = render.GetPrefilterCubemap();
                material->SetTexture(5, texture);
                texture.id = render.GetBRDF();
                material->SetTexture(6, texture);

                sphere->SetMaterial(material);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{x, y, 0.0f});
                sphere->SetTransform(transform);
                actors_.push_back(sphere);
            }
        }
    }

    if (true)
    {
        auto &textures = meshes_map["sphere"]->GetTextures();
        textures[0].id = render.GetTexture("assets/textures/sphere/rustediron2_basecolor.png");
        // textures.push_back(texture);
        Texture texture;
        texture.id = render.GetTexture("assets/textures/sphere/rustediron2_normal.png");
        textures.push_back(texture);
        texture.id = render.GetTexture("assets/textures/sphere/rustediron2_metallic.png");
        textures.push_back(texture);
        texture.id = render.GetTexture("assets/textures/sphere/rustediron2_roughness.png");
        textures.push_back(texture);

        auto sphere = std::make_shared<Actor>();
        sphere->AddMesh(meshes_map["sphere"]);
        sphere->SetShader(shaders_map["pbr"]);
        auto material = std::make_shared<Material>();
        material->SetFloat("ao", 1.0f);

        texture.id = render.GetConvolutionCubemap();
        material->SetTexture(4, texture);
        texture.id = render.GetPrefilterCubemap();
        material->SetTexture(5, texture);
        texture.id = render.GetBRDF();
        material->SetTexture(6, texture);

        sphere->SetMaterial(material);
        actors_.push_back(sphere);
    }
}