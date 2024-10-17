#include "SceneCloth.hpp"

#include "BasicActor/Cloth.hpp"
#include "render/BasicGeometry/Plane.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"

#include <cstddef>
#include <memory>

using namespace std;

void SceneCloth::Init()
{
    InitCloth();
    Scene::Init();
}

void SceneCloth::InitCloth()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    auto plane = make_shared<Plane>();
    plane->SetDynamic();

    plane->Init(10, 10.f);
    cloth_ = make_shared<Cloth>();
    cloth_->AddMesh(plane);
    cloth_->SetShader(shaders_map["phong"]);

    auto material = std::make_shared<Material>();
    Texture texture;
    texture.id = render.GetTexture("default_red");
    material->SetTexture(0, texture);
    material->SetFloat("ambientStrength", 0.00f);
    material->SetFloat("specularStrength", 0.5f);
    material->SetFloat("specularPow", 255.f);

    cloth_->SetMaterial(material);

    cloth_->GeneratePoints();
    cloth_->GenerateSegments();
    cloth_->InitPhysics();

    actors_.push_back(cloth_);
}

void SceneCloth::Update()
{
    Scene::Update();
}