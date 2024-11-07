#include "SceneClothSphere.hpp"

#include "BasicActor/Cloth.hpp"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/string_cast.hpp"
#include "math/Math.hpp"
#include "physics/Particle.hpp"
#include "render/BasicGeometry/Plane.hpp"
#include "render/BasicGeometry/Sphere.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"
#include <glm/gtx/matrix_decompose.hpp>

#include <cstddef>
#include <memory>

using namespace std;
using namespace math;


void SceneClothSphere::Init()
{
    InitCloth();
    InitSphere();
    Scene::Init();
}

void SceneClothSphere::InitCloth()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    auto plane = make_shared<Plane>();
    plane->SetDynamic();

    plane->Init(10, 10.f);
    auto &vertices = plane->GetVertices();
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].position_.y += 2.1f;
    }
    plane->ReComputeNormal();
    plane->UpdateVBO();

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

void SceneClothSphere::InitSphere()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    sphere_ = make_shared<Actor>();
    sphere_->SetShader(shaders_map["phong"]);

    auto material = std::make_shared<Material>();
    Texture texture;
    texture.id = render.GetTexture("default_blue");
    material->SetTexture(0, texture);
    material->SetFloat("ambientStrength", 0.00f);
    material->SetFloat("specularStrength", 0.5f);
    material->SetFloat("specularPow", 255.f);

    sphere_->SetMaterial(material);

    auto mesh = make_shared<Sphere>();
    mesh->SetDynamic();
    mesh->Init(36u, 18u, 2.f);
    sphere_->AddMesh(mesh);

    auto transform = glm::mat4(1.f);
    sphere_->SetTransform(transform);

    actors_.push_back(sphere_);
}

void SceneClothSphere::Update()
{
    Scene::Update();
    UpdateSphere();
    CollideClothSphere();
}

void SceneClothSphere::CollideClothSphere()
{
    auto &particles = cloth_->cloth_.particles_;
    auto transform = sphere_->GetTransform();
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    Eigen::Vector3f center = ToEigen(translation);

    float radius = 2.f;

    for (auto &particle : particles)
    {
        Eigen::Vector3f dir = particle->pos_ - center;
        float dist = dir.norm();
        if (dist < radius)
        {
            dir.normalize();
            Eigen::Vector3f A = center + radius * dir;
            particle->velocity_ = (A - particle->pos_) / 0.016f;
            // particle->mass_inv_ = 0.f;
        }
    }

    auto meshes = cloth_->GetMeshes();
    auto mesh = dynamic_pointer_cast<MeshTBN>(meshes[0]);
    auto &vertices = mesh->GetVertices();
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].position_ = ToGLM(particles[i]->pos_);
    }
    mesh->ReComputeNormal();
    mesh->UpdateVBO();
}

void SceneClothSphere::UpdateSphere()
{
    bool is_selected = Render::GetInstance().GetIsSelected();
    if (is_selected)
    {
        glm::vec3 mouse = glm::vec3(Render::GetInstance().mouse_pos_x, Render::GetInstance().mouse_pos_y, 0.f);
        glm::vec3 translate = Render::GetInstance().GetTranslate();
        mouse -= translate;

        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(sphere_->GetTransform(), scale, rotation, translation, skew, perspective);

        auto camera = Render::GetInstance().GetCamera();
        glm::mat4 view = camera->GetView();
        glm::mat4 proj = camera->GetProj();
        glm::mat4 mvp = proj * view;
        glm::mat4 mvp_inverse = glm::inverse(mvp);

        glm::vec3 screen_pos =
            glm::project(translation, view, proj, glm::vec4(0, 0, camera->GetWidth(), camera->GetHeight()));
        mouse.z = screen_pos.z;

        mouse[0] = 2 * mouse[0] / camera->GetWidth() - 1.f;
        mouse[1] = 1-2 * mouse[1] / camera->GetHeight();
        mouse[2] = 2 * mouse[2] - 1.f;
        glm::vec4 mouse_h = glm::vec4(mouse, 1);
        glm::vec4 model_dx = mvp_inverse * mouse_h;
        model_dx /= model_dx[3];

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(model_dx));

        sphere_->SetTransform(transform);
        sphere_->GetPrimitives()[0]->SetTransform(transform);
    }
}