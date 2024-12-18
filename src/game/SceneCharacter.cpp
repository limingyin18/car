#include "SceneCharacter.hpp"

#include "Actor/ActorSkeletal.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Render.hpp"

#include <SDL2/SDL.h>

void SceneCharacter::Init()
{
    InitHuman();
    Scene::Init();
}

void SceneCharacter::InitHuman()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    human_ = std::make_shared<ActorSkeletal>();
    human_->SetModelPath("assets/human_test/human_test.gltf");
    human_->SetShader(shaders_map["skeletal_phong"]);
    // human_->AddAnimation("assets/human_test/human_test_hippop.gltf");

    auto material = std::make_shared<Material>();
    Texture texture;
    texture.id = render.GetTexture("default_white");
    material->SetTexture(0, texture);
    material->SetFloat("ambientStrength", 0.0f);
    human_->SetMaterial(material);
    // glm::mat4 trans = glm::scale(glm::mat4(1.f), glm::vec3(1.f));
    // human_->SetTransform(trans);

    actors_.push_back(human_);
}

void SceneCharacter::ProcessInput(SDL_Event *event)
{
    static int index = 0;
    switch (event->type)
    {
    case SDL_KEYDOWN:
        switch (event->key.keysym.sym)
        {
        case SDLK_f:
            ++index;
            index = index % 2;
            if (index == 0)
            {
                human_->SetAnimated(true);
                human_->SwitchAnimation(index);
            }
            else
            {
                human_->SetAnimated(false);
            }
            break;
        default:
            break;
        }
    default:
        break;
    }
}