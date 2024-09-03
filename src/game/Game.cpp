#include "Game.hpp"

#include "Actor/ActorInstance.hpp"
#include "Actor/ActorSkeletal.hpp"
#include "Actor/ActorSkeletalIndirect.hpp"
#include "Actor/ActorSkeletalIndirectLod.hpp"
#include "Actor/ActorSkeletalInstance.hpp"
#include "glm/trigonometric.hpp"
#include "render/Render.hpp"

#include <cstdint>
#include <memory>
#include <spdlog/spdlog.h>

void Game::Init(const std::shared_ptr<Render> &render)
{
    spdlog::debug("Game Init");

    render_ = render;
    auto &shaders_map = render_->GetShadersMap();
    auto &meshes_map = render_->GetMeshesMap();

    house_ = std::make_shared<Actor>();
    house_->SetModelPath("assets/Sponza/glTF/Sponza.gltf");
    house_->SetShader(shaders_map["phong"]);
    actors_.push_back(house_);

    cube_ = std::make_shared<ActorIndirect>();
    cube_->AddMesh(meshes_map["cube"]);
    cube_->SetShader(shaders_map["phong_indirect"]);
    cube_->SetShaderCulling(shaders_map["culling"]);
    uint32_t instance_count = 100;
    std::vector<glm::mat4> instance_transforms(instance_count * instance_count);
    for (uint32_t i = 0; i < instance_count; i++)
    {
        for (uint32_t j = 0; j < instance_count; j++)
        {
            uint32_t index = j + i * instance_count;
            auto &transform = instance_transforms[index];
            transform = glm::scale(glm::mat4(1.0f), glm::vec3(100.f, 100.f, 100.f));
            transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(1, 0, 0));
            transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f, 0.0f, j * 1.0f)) * transform;
        }
    }
    // cube_->SetInstanceTransforms(instance_transforms);
    // actors_.push_back(cube_);

    fox_ = std::make_shared<ActorSkeletalIndirectLod>();
    // fox_ = std::make_shared<ActorSkeletal>();
    fox_->SetModelPath("assets/Shield/shield.gltf");
    // fox_->SetModelPath("assets/fox/Fox.gltf");
    // fox_->SetShader(shaders_map["skeletal_phong"]);
    fox_->SetTransform(instance_transforms[0]);
    fox_->SetShader(shaders_map["skeletal_phong_indirect_lod"]);
    fox_->SetShaderCulling(shaders_map["culling_lod"]);
    fox_->SetInstanceTransforms(instance_transforms);
    actors_.push_back(fox_);

    for (auto &actor : actors_)
    {
        actor->Init();
    }
}

void Game::Update()
{
    for (auto &actor : actors_)
    {
        actor->Update();
    }
}

void Game::SwitchFoxAnimation()
{
    static uint32_t index = 0;
    index = (index + 1) % fox_->GetAnimationCount();
    fox_->SwitchAnimation(index);
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