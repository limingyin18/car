#include "Scene.hpp"

#include "Actor/IActor.hpp"
#include "BasicActor/Cloth.hpp"
#include "BasicActor/Skybox.hpp"
#include "render/BasicGeometry/Plane.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"

using namespace std;

void Scene::Init()
{
    InitSkybox();
    InitPlane();

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

void Scene::UpdatePhysics()
{
    for (auto &actor : actors_)
    {
        actor->UpdatePhysics();
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

void Scene::InitPlane()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    plane_ = std::make_shared<Actor>();
    plane_->AddMesh(meshes_map["plane"]);
    plane_->SetShader(shaders_map["phong"]);

    auto material = std::make_shared<Material>();
    Texture texture;
    texture.id = render.GetTexture("uvgrid");
    material->SetTexture(0, texture);
    material->SetFloat("ambientStrength", 0.0f);
    plane_->SetMaterial(material);

    plane_->SetTransform(glm::scale(glm::mat4(1.0f), glm::vec3(8.f)));

    actors_.push_back(plane_);
    plane_ = std::make_shared<Actor>();

    actors_.push_back(plane_);
}