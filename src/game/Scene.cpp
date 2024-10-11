#include "Scene.hpp"

#include "Actor/IActor.hpp"
#include "BasicActor/Skybox.hpp"

using namespace std;

void Scene::Init()
{
    InitSkybox();

    for (auto &actor : actors_)
    {
        actor->Init();
    }
}

void Scene::Update()
{
    for (auto &actor : actors_)
    {
        actor->Update();
    }
}

std::vector<std::shared_ptr<IPrimitive>> Scene::GetPrimitives() const
{
    std::vector<std::shared_ptr<IPrimitive>> primitives;
    for (auto &actor : actors_)
    {
        auto actor_primitives = actor->GetPrimitives();
        primitives.insert(primitives.end(), actor_primitives.begin(), actor_primitives.end());
    }
    return primitives;
}

void Scene::InitSkybox()
{
    skybox_ = std::make_shared<Skybox>();
    actors_.push_back(skybox_);
}