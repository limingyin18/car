#include "SceneOneSpring.hpp"

#include "Actor/ActorInstance.hpp"
#include "render/BasicGeometry/Sphere.hpp"
#include "render/Material/Material.hpp"
#include "render/Render.hpp"

#include "physics/Particle.hpp"
#include "physics/PhysicsSystem.hpp"

#include <glm/gtx/matrix_decompose.hpp>

#include "math/Math.hpp"

using namespace std;
using namespace Eigen;
using namespace math;

void SceneOneSpring::Init()
{
    InitPoints();
    InitSpring();
    Scene::Init();
}

void SceneOneSpring::InitPoints()
{
    // Create points
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    auto &physics_system = PhysicsSystem::GetInstance();

    auto sphere_mesh = make_shared<Sphere>();
    sphere_mesh->Init(36u, 18u, 0.1f);

    auto sphere = std::make_shared<ActorInstance>();
    sphere->AddMesh(sphere_mesh);
    sphere->SetShader(shaders_map["phong_instance"]);
    auto material = std::make_shared<Material>();
    Texture texture;
    texture.id = render.GetTexture("default_white");
    material->SetTexture(0, texture);
    sphere->SetMaterial(material);

    std::vector<glm::mat4> instance_transforms;
    glm::vec3 translation{0.0f, 0.0f, 0.0f};
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation);
    instance_transforms.push_back(transform);
    auto particle0 = make_shared<Particle>(ToEigen(translation));
    physics_system.AddParticle(particle0);

    translation = {1.0f, 0.0f, 0.0f};
    transform = glm::translate(glm::mat4(1.0f), translation);
    instance_transforms.push_back(transform);
    auto particle1 = make_shared<Particle>(ToEigen(translation));
    physics_system.AddParticle(particle1);

    sphere->SetInstanceTransforms(instance_transforms);

    points_ = sphere;
    actors_.push_back(points_);
}

void SceneOneSpring::InitSpring()
{
    auto &physics_system = PhysicsSystem::GetInstance();

    edges_.push_back(Edge{0, 1});

    auto &particles = physics_system.particles_;
    for (uint32_t i = 0; i < edges_.size(); ++i)
    {
        auto edge = edges_[i];
        auto particle0 = particles[edge.vertex_id_start_];
        auto particle1 = particles[edge.vertex_id_end_];
        physics_system.AddSpring(particle0, particle1);
    }
    physics_system.particles_[1]->pos_ = Vector3f{1.5f, 0.0f, 0.0f};
}

void SceneOneSpring::Update()
{
    auto points = std::dynamic_pointer_cast<ActorInstance>(points_);
    auto &instance_transforms = points->GetInstanceTransforms();

    instance_transforms[0] = glm::translate(glm::mat4(1.0f), ToGLM(PhysicsSystem::GetInstance().particles_[0]->pos_));
    instance_transforms[1] = glm::translate(glm::mat4(1.0f), ToGLM(PhysicsSystem::GetInstance().particles_[1]->pos_));
    points->SetInstanceTransforms(instance_transforms);

    Scene::Update();
}