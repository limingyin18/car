#include "Game.hpp"

#include "Actor/ActorInstance.hpp"
#include "Actor/ActorSkeletal.hpp"
#include "Actor/ActorSkeletalIndirect.hpp"
#include "Actor/ActorSkeletalIndirectLod.hpp"
#include "Actor/ActorSkeletalInstance.hpp"
#include "Skybox.hpp"
#include "glm/trigonometric.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"

#include <cstdint>
#include <memory>
#include <spdlog/spdlog.h>

void Game::Init()
{
    spdlog::debug("Game Init");

    // TestBasicGeometry();
    InitShields();
    InitHouse();

    skybox_ = std::make_shared<Skybox>();
    actors_.push_back(skybox_);

    for (auto &actor : actors_)
    {
        actor->Init();
    }
}

void Game::Update()
{
    if (index == 1)
    {
        auto &transforms = shield_->GetInstanceTransforms();
        for (auto &transform : transforms)
        {
            transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.5f * 0.016f));
        }
    }
    for (auto &actor : actors_)
    {
        actor->Update();
    }
}

void Game::SwitchAnimation()
{
    index = (index + 1) % shield_->GetAnimationCount();
    shield_->SwitchAnimation(index);

    arrow_->SwitchAnimation(index);
}

std::vector<std::shared_ptr<Primitive>> Game::GetPrimitives() const
{
    std::vector<std::shared_ptr<Primitive>> primitives;
    for (auto &actor : actors_)
    {
        auto actor_primitives = actor->GetPrimitives();
        primitives.insert(primitives.end(), actor_primitives.begin(), actor_primitives.end());
    }
    return primitives;
}

void Game::InitShields()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    uint32_t instance_count = 10;
    std::vector<glm::mat4> instance_transforms(instance_count * instance_count);
    for (uint32_t i = 0; i < instance_count; i++)
    {
        for (uint32_t j = 0; j < instance_count; j++)
        {
            uint32_t index = j + i * instance_count;
            auto &transform = instance_transforms[index];
            transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.f, 1.f, 1.f));
            transform = glm::rotate(transform, glm::radians(0.f), glm::vec3(1, 0, 0));
            transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f, 0.0f, j * 1.0f)) * transform;
        }
    }

    shield_ = std::make_shared<ActorSkeletalIndirectLod>();
    shield_->SetModelPath("assets/Shield/sword.gltf");
    shield_->SetShader(shaders_map["skeletal_phong_indirect_lod"]);
    shield_->SetShaderCulling(shaders_map["culling_lod"]);
    shield_->SetInstanceTransforms(instance_transforms);
    shield_->AddAnimation("assets/Shield/idle.gltf");
    shield_->AddAnimation("assets/Shield/walk.gltf");
    shield_->AddAnimation("assets/Shield/slash.gltf");
    shield_->AddAnimation("assets/Shield/death.gltf");

    actors_.push_back(shield_);
}

void Game::InitArrows()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    uint32_t instance_count = 10;
    std::vector<glm::mat4> instance_transforms(instance_count * instance_count);
    for (uint32_t i = 0; i < instance_count; i++)
    {
        for (uint32_t j = 0; j < instance_count; j++)
        {
            uint32_t index = j + i * instance_count;
            auto &transform = instance_transforms[index];
            transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.f, 1.f, 1.f));
            transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0, 1, 0));
            transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f, 0.0f, 100 + 10 + j * 1.0f)) * transform;
        }
    }

    arrow_ = std::make_shared<ActorSkeletalIndirectLod>();
    arrow_->SetModelPath("assets/Arrow/arrow.gltf");
    arrow_->SetShader(shaders_map["skeletal_phong_indirect_lod"]);
    arrow_->SetShaderCulling(shaders_map["culling_lod"]);
    arrow_->SetInstanceTransforms(instance_transforms);
    arrow_->AddAnimation("assets/arrow/idle.gltf");
    arrow_->AddAnimation("assets/arrow/walk.gltf");
    arrow_->AddAnimation("assets/arrow/attack.gltf");
    arrow_->AddAnimation("assets/arrow/death.gltf");

    actors_.push_back(arrow_);
}

void Game::InitHouse()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    house_ = std::make_shared<Actor>();
    // house_->SetModelPath("assets/Sponza/glTF/Sponza.gltf");
    house_->SetModelPath("assets/ABeautifulGame/glTF/ABeautifulGame.gltf");
    house_->SetShader(shaders_map["pbr"]);

    Texture texture;
    texture.id = render.GetConvolutionCubemap();
    auto material = std::make_shared<Material>();
    material->SetTexture(3, texture);
    texture.id = render.GetPrefilterCubemap();
    material->SetTexture(4, texture);
    texture.id = render.GetBRDF();
    material->SetTexture(5, texture);
    house_->SetMaterial(material);

    // house_->SetTransform(glm::scale(glm::mat4(1.0f), glm::vec3(10.f, 10.f, 10.f)));

    actors_.push_back(house_);
}

void Game::InitCubes()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    uint32_t instance_count = 10;
    std::vector<glm::mat4> instance_transforms(instance_count * instance_count);
    for (uint32_t i = 0; i < instance_count; i++)
    {
        for (uint32_t j = 0; j < instance_count; j++)
        {
            uint32_t index = j + i * instance_count;
            auto &transform = instance_transforms[index];
            transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
            transform = glm::rotate(transform, glm::radians(0.f), glm::vec3(0, 1, 0));
            transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f, 0.0f, j * 1.0f)) * transform;
        }
    }

    cube_ = std::make_shared<ActorIndirect>();
    cube_->AddMesh(meshes_map["cube"]);
    cube_->SetShader(shaders_map["phong_indirect"]);
    cube_->SetShaderCulling(shaders_map["culling"]);
    cube_->SetInstanceTransforms(instance_transforms);
    actors_.push_back(cube_);
}