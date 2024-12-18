#include "SceneTwoTriangle.hpp"

#include "Actor/Actor.hpp"
#include "Eigen/Core"
#include "math/Math.hpp"
#include "physics/Particle.hpp"
#include "physics/collision/CCD.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"
#include "tools/Tool.hpp"

#include <memory>

using namespace std;

void SceneTwoTriangle::Init()
{
    InitTwoTriangle();
    Scene::Init();
}

void SceneTwoTriangle::InitTwoTriangle()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();

    {
        auto mesh = make_shared<Mesh>();

        std::vector<Vertex> vertices{Vertex{glm::vec3{-0.5f, 0.5f, 0.5f}}, Vertex{glm::vec3{-0.5f, 1.f, 0.f}},
                                     Vertex{glm::vec3{-0.5f, 0.5f, -0.5f}}};
        std::vector<uint32_t> indices{0, 1, 2};

        particles_a_.resize(vertices.size());
        for (int i = 0; i < 3; i++)
        {
            particles_a_[i] = make_shared<Particle>();
            particles_a_[i]->pos_ = math::ToEigen(vertices[i].position_);
            particles_a_[i]->velocity_ = Eigen::Vector3f(1.f, 0.f, 0.f);
        }

        mesh->SetDynamic();
        mesh->Init(vertices, indices);
        mesh->ReComputeNormal();

        auto material = std::make_shared<Material>();
        Texture texture;
        texture.id = render.GetTexture("default_blue");
        material->SetTexture(0, texture);
        material->SetFloat("ambientStrength", 0.3f);
        material->SetFloat("specularStrength", 0.5f);
        material->SetFloat("specularPow", 255.f);

        triangle_a_ = make_shared<Actor>();
        triangle_a_->SetShader(shaders_map["phong"]);
        triangle_a_->AddMesh(mesh);
        triangle_a_->SetMaterial(material);
        actors_.push_back(triangle_a_);
    }

    {
        auto mesh = make_shared<Mesh>();

        std::vector<Vertex> vertices{Vertex{glm::vec3{0.5f, 0.5f, 0.5f}}, Vertex{glm::vec3{0.5f, 1.f, 0.f}},
                                     Vertex{glm::vec3{0.5f, 0.5f, -0.5f}}};
        std::vector<uint32_t> indices{0, 1, 2};

        particles_b_.resize(vertices.size());
        for (int i = 0; i < 3; i++)
        {
            particles_b_[i] = make_shared<Particle>();
            particles_b_[i]->pos_ = math::ToEigen(vertices[i].position_);
            particles_b_[i]->velocity_ = Eigen::Vector3f(-1.f, 0.f, 0.f);
        }

        mesh->SetDynamic();
        mesh->Init(vertices, indices);
        mesh->ReComputeNormal();

        auto material = std::make_shared<Material>();
        Texture texture;
        texture.id = render.GetTexture("default_red");
        material->SetTexture(0, texture);
        material->SetFloat("ambientStrength", 0.3f);
        material->SetFloat("specularStrength", 0.5f);
        material->SetFloat("specularPow", 255.f);

        triangle_b_ = make_shared<Actor>();
        triangle_b_->SetShader(shaders_map["phong"]);
        triangle_b_->AddMesh(mesh);
        triangle_b_->SetMaterial(material);

        actors_.push_back(triangle_b_);
    }
}

void SceneTwoTriangle::Update()
{
    Scene::Update();

    {
        auto meshes_a = triangle_a_->GetMeshes();
        auto mesh_a = dynamic_pointer_cast<Mesh>(meshes_a[0]);
        auto &vertices_a = mesh_a->GetVertices();
        for (int i = 0; i < 3; i++)
        {
            vertices_a[i].position_ = math::ToGLM(particles_a_[i]->pos_);
        }

        mesh_a->ReComputeNormal();
        mesh_a->UpdateVBO();
    }
    {
        auto meshes_b = triangle_b_->GetMeshes();
        auto mesh_b = dynamic_pointer_cast<Mesh>(meshes_b[0]);
        auto &vertices_b = mesh_b->GetVertices();
        for (int i = 0; i < 3; i++)
        {
            vertices_b[i].position_ = math::ToGLM(particles_b_[i]->pos_);
        }
        mesh_b->ReComputeNormal();
        mesh_b->UpdateVBO();
    }
}
void SceneTwoTriangle::UpdatePhysics()
{
    for (auto &actor : actors_)
    {
        actor->UpdatePhysics();
    }

    {
        spdlog::info("particles_a_");

        spdlog::info("{}", Tool::EigenToString(particles_a_[0]->pos_));
        spdlog::info("{}", Tool::EigenToString(particles_a_[1]->pos_));
        spdlog::info("{}", Tool::EigenToString(particles_a_[2]->pos_));

        spdlog::info("{}", Tool::EigenToString(particles_a_[0]->velocity_));
        spdlog::info("{}", Tool::EigenToString(particles_a_[1]->velocity_));
        spdlog::info("{}", Tool::EigenToString(particles_a_[2]->velocity_));

        spdlog::info("particles_b_");

        spdlog::info("{}", Tool::EigenToString(particles_b_[0]->pos_));
        spdlog::info("{}", Tool::EigenToString(particles_b_[1]->pos_));
        spdlog::info("{}", Tool::EigenToString(particles_b_[2]->pos_));

        spdlog::info("{}", Tool::EigenToString(particles_b_[0]->velocity_));
        spdlog::info("{}", Tool::EigenToString(particles_b_[1]->velocity_));
        spdlog::info("{}", Tool::EigenToString(particles_b_[2]->velocity_));
    }

    auto t = physics::CCDTriangleTriangle(*particles_a_[0], *particles_a_[1], *particles_a_[2], *particles_b_[0],
                                          *particles_b_[1], *particles_b_[2], 0.016f);
    if (t.has_value())
    {
        spdlog::info("collision time: {}", t.value());

        spdlog::info("particles_a_");
        spdlog::info("{}", Tool::EigenToString(particles_a_[0]->velocity_));
        spdlog::info("{}", Tool::EigenToString(particles_a_[1]->velocity_));
        spdlog::info("{}", Tool::EigenToString(particles_a_[2]->velocity_));

        spdlog::info("particles_b_");
        spdlog::info("{}", Tool::EigenToString(particles_b_[0]->velocity_));
        spdlog::info("{}", Tool::EigenToString(particles_b_[1]->velocity_));
        spdlog::info("{}", Tool::EigenToString(particles_b_[2]->velocity_));
    }

    {
        for (int i = 0; i < 3; i++)
        {
            particles_a_[i]->pos_ += particles_a_[i]->velocity_ * 0.016f;
        }
        for (int i = 0; i < 3; i++)
        {
            particles_b_[i]->pos_ += particles_b_[i]->velocity_ * 0.016f;
        }
    }
}