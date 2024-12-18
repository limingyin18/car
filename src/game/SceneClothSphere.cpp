#include "SceneClothSphere.hpp"

#include "BasicActor/Cloth.hpp"
#include "game/Scene.hpp"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/string_cast.hpp"
#include "math/Math.hpp"
#include "physics/Particle.hpp"
#include "physics/collision/CCD.hpp"
#include "render/BasicGeometry/Plane.hpp"
#include "render/BasicGeometry/Sphere.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"
#include "tools/Tool.hpp"
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

    plane->Init(3, 3.f);
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
    mesh->Init(10u, 10u, 2.f);
    sphere_->AddMesh(mesh);

    auto transform = glm::mat4(1.f);
    sphere_->SetTransform(transform);

    actors_.push_back(sphere_);
}

void SceneClothSphere::Update()
{
    Scene::Update();
    UpdateSphere();
}

void SceneClothSphere::UpdatePhysics()
{
    Scene::UpdatePhysics();
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

    // ccd
    {
        auto mesh = dynamic_pointer_cast<MeshTBN>(cloth_->GetMeshes()[0]);
        auto &vertices = mesh->GetVertices();
        auto &indices = mesh->GetIndices();

        auto mesh_sphere = dynamic_pointer_cast<Sphere>(sphere_->GetMeshes()[0]);
        auto &vertices_sphere = mesh_sphere->GetVertices();
        auto &indices_sphere = mesh_sphere->GetIndices();

        for (uint32_t i = 0; i < indices.size(); i += 3)
        {
            uint32_t index0 = indices[i + 0];
            uint32_t index1 = indices[i + 1];
            uint32_t index2 = indices[i + 2];
            auto &p0 = *cloth_->cloth_.particles_[index0];
            auto &p1 = *cloth_->cloth_.particles_[index1];
            auto &p2 = *cloth_->cloth_.particles_[index2];

            for (uint32_t j = 0; j < indices_sphere.size(); j += 3)
            {
                uint32_t index0_sphere = indices_sphere[j + 0];
                uint32_t index1_sphere = indices_sphere[j + 1];
                uint32_t index2_sphere = indices_sphere[j + 2];
                auto p3 =
                    Particle(ToEigen(vertices_sphere[index0_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);
                auto p4 =
                    Particle(ToEigen(vertices_sphere[index1_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);
                auto p5 =
                    Particle(ToEigen(vertices_sphere[index2_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);
                auto t = physics::CCDTriangleTriangle(p0, p1, p2, p3, p4, p5, 0.016f);
                if (t.has_value())
                {
                    spdlog::info("collision time: {}", t.value());
                }
            }
        }
    }

    for (auto &particle : particles)
    {
        particle->pos_ += particle->velocity_ * 0.016f;
    }

    // sdf
    {
        // for (auto &particle : particles)
        // {
        //     Eigen::Vector3f dir = particle->pos_ - center;
        //     float dist = dir.norm();
        //     if (dist < radius)
        //     {
        //         int a = 1;
        //         spdlog::error("collision");
        //         spdlog::debug("{}", Tool::EigenToString(particle->pos_));
        //         spdlog::debug("{}", Tool::EigenToString(particle->velocity_));
        //         throw std::runtime_error("collision");
        //         // dir.normalize();
        //         // Eigen::Vector3f A = center + radius * dir;
        //         // particle->velocity_ = (A - particle->pos_) / 0.016f;
        //         // particle->mass_inv_ = 0.f;
        //     }
        // }
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
        mouse[1] = 1 - 2 * mouse[1] / camera->GetHeight();
        mouse[2] = 2 * mouse[2] - 1.f;
        glm::vec4 mouse_h = glm::vec4(mouse, 1);
        glm::vec4 model_dx = mvp_inverse * mouse_h;
        model_dx /= model_dx[3];

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(model_dx));

        sphere_->SetTransform(transform);
        sphere_->GetPrimitives()[0]->SetTransform(transform);
    }
}