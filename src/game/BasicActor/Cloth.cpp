#include "Cloth.hpp"

#include "game/Actor/ActorInstance.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "math/Math.hpp"
#include "physics/Particle.hpp"
#include "physics/PhysicsSystem.hpp"
#include "render/BasicGeometry/Cube.hpp"
#include "render/BasicGeometry/Sphere.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"
#include <cassert>
#include <glm/gtx/matrix_decompose.hpp>

#include "game/Game.hpp"

#include <cstdint>
#include <memory>
#include <unordered_set>

using namespace std;
using namespace math;

std::ostream &operator<<(std::ostream &os, const Edge &edge)
{
    os << "{" << edge.vertex_id_start_ << ", " << edge.vertex_id_end_ << "}";
    return os;
}

Triple::Triple(uint32_t vertex_id_start, uint32_t vertex_id_end, uint32_t triangle_id)
    : vertex_id_start_(vertex_id_start), vertex_id_end_(vertex_id_end), triangle_id_(triangle_id)
{
    if (vertex_id_start > vertex_id_end)
    {
        std::swap(vertex_id_start_, vertex_id_end_);
    }
}

std::ostream &operator<<(std::ostream &os, const Triple &triple)
{
    os << "{" << triple.vertex_id_start_ << ", " << triple.vertex_id_end_ << ", " << triple.triangle_id_ << "}";
    return os;
}

bool Triple::operator<(const Triple &rhs) const
{
    if (this->vertex_id_start_ < rhs.vertex_id_start_)
    {
        return true;
    }
    else if (this->vertex_id_start_ == rhs.vertex_id_start_)
    {
        if (this->vertex_id_end_ < rhs.vertex_id_end_)
        {
            return true;
        }
        else if (this->vertex_id_end_ == rhs.vertex_id_end_)
        {
            if (this->triangle_id_ < rhs.triangle_id_)
            {
                return true;
            }
        }
    }

    return false;
}

void Cloth::GenerateSegments()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();
    auto &game = Game::GetInstance();
    auto &actors = game.GetActors();

    auto mesh = dynamic_pointer_cast<MeshTBN>(meshes_[0]);
    auto &vertices = mesh->GetVertices();
    auto &indices = mesh->GetIndices();

    vector<Triple> triples;
    triples.reserve(indices.size());
    for (uint32_t i = 0; i < indices.size(); i += 3)
    {
        uint32_t index0 = indices[i + 0];
        uint32_t index1 = indices[i + 1];
        uint32_t index2 = indices[i + 2];

        Triple triple0(index0, index1, i / 3);
        triples.push_back(triple0);

        Triple triple1(index1, index2, i / 3);
        triples.push_back(triple1);

        Triple triple2(index2, index0, i / 3);
        triples.push_back(triple2);
    }

    std::sort(triples.begin(), triples.end());

    for (size_t i = 0; i < triples.size(); ++i)
    {
        auto triple = triples[i];
        Edge edge;
        edge.vertex_id_start_ = triple.vertex_id_start_;
        edge.vertex_id_end_ = triple.vertex_id_end_;
        if (edges_.empty())
        {
            edges_.push_back(edge);
            continue;
        }
        if (edges_.back().vertex_id_start_ == edge.vertex_id_start_ &&
            edges_.back().vertex_id_end_ == edge.vertex_id_end_)
        {
            adjacency_list_.emplace_back(triples[i - 1].triangle_id_, triple.triangle_id_);
            continue;
        }

        edges_.push_back(edge);
    }

    auto cube_mesh = make_shared<Cube>();
    cube_mesh->Init();
    segments_.reserve(indices.size());
    auto cube = std::make_shared<ActorInstance>();
    cube->AddMesh(cube_mesh);
    cube->SetShader(shaders_map["phong_instance"]);
    auto material = std::make_shared<Material>();
    Texture texture;
    texture.id = render.GetTexture("default_blue");
    material->SetTexture(0, texture);
    cube->SetMaterial(material);
    segments_.push_back(cube);

    std::vector<glm::mat4> instance_transforms;
    instance_transforms.reserve(triples.size());
    for (auto &&edge : edges_)
    {
        glm::vec3 pos = vertices[edge.vertex_id_start_].position_ + vertices[edge.vertex_id_end_].position_;
        pos /= 2.0f;
        glm::mat4 transform = glm::mat4(1.f);
        // position
        transform = glm::translate(transform, pos);
        // rotation
        glm::vec3 direction = vertices[edge.vertex_id_end_].position_ - vertices[edge.vertex_id_start_].position_;
        glm::vec3 up = glm::vec3(0.f, 0.f, 1.f);
        glm::vec3 axis = glm::vec3(0.f, 1.f, 0.f);
        float angle = glm::angle(up, glm::normalize(direction));
        transform = glm::rotate(transform, angle, axis);
        // scale
        transform = glm::scale(transform, glm::vec3(0.1f, 0.1f,
                                                    glm::distance(vertices[edge.vertex_id_start_].position_,
                                                                  vertices[edge.vertex_id_end_].position_)));
        instance_transforms.push_back(transform);
    }
    cube->SetInstanceTransforms(instance_transforms);

    for (auto &&segment : segments_)
    {
        // actors.push_back(segment);
    }
}

void Cloth::GeneratePoints()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();
    auto &game = Game::GetInstance();
    auto &actors = game.GetActors();

    auto mesh = dynamic_pointer_cast<MeshTBN>(meshes_[0]);
    auto &vertices = mesh->GetVertices();

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
    m_ = vertices.size();
    for (auto &vertex : vertices)
    {
        glm::mat4 transform = glm::translate(vertex.position_);
        transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.f));
        instance_transforms.push_back(transform);
    }
    sphere->SetInstanceTransforms(instance_transforms);

    points_ = sphere;
    // actors.push_back(points_);
}

void Cloth::Init()
{
    Actor::Init();
}

void Cloth::Update()
{
    cloth_.Update();

    auto mesh = dynamic_pointer_cast<MeshTBN>(meshes_[0]);
    auto &vertices = mesh->GetVertices();
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].position_ = ToGLM(cloth_.particles_[i]->pos_);
    }
    mesh->ReComputeNormal();
    mesh->UpdateVBO();

    Actor::Update();
}

void Cloth::InitPhysics()
{
    auto &instance_transforms = std::dynamic_pointer_cast<ActorInstance>(points_)->GetInstanceTransforms();
    for (uint32_t i = 0; i < instance_transforms.size(); ++i)
    {
        auto transform = instance_transforms[i];
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transform, scale, rotation, translation, skew, perspective);

        auto particle = make_shared<Particle>(ToEigen(translation));
        particle->mass_inv_ = 1.f / 0.001f;
        // if (i == 0 || i == 9 || i == 90 || i == 99)
        if (i == 0 || i == 9 )
        {
            particle->mass_inv_ = 0.f;
        }

        // if (i == 0 || i == 2 || i == 6 || i == 8)
        // {
        //     particle->mass_inv_ = 0.f;
        // }
        cloth_.AddParticle(particle);
    }

    auto &particles = cloth_.particles_;
    for (uint32_t i = 0; i < edges_.size(); ++i)
    {
        auto edge = edges_[i];
        auto particle0 = particles[edge.vertex_id_start_];
        auto particle1 = particles[edge.vertex_id_end_];
        cloth_.AddSpring(edge.vertex_id_start_, edge.vertex_id_end_, particle0, particle1, 1.f);
    }

    auto mesh = dynamic_pointer_cast<MeshTBN>(meshes_[0]);
    auto &vertices = mesh->GetVertices();
    auto &indices = mesh->GetIndices();
    for (auto &&adjacency : adjacency_list_)
    {
        uint32_t x00 = indices[3 * adjacency.first + 0];
        uint32_t x10 = indices[3 * adjacency.first + 1];
        uint32_t x20 = indices[3 * adjacency.first + 2];

        uint32_t x01 = indices[3 * adjacency.second + 0];
        uint32_t x11 = indices[3 * adjacency.second + 1];
        uint32_t x21 = indices[3 * adjacency.second + 2];

        std::unordered_set<uint32_t> s0;
        s0.insert(x00);
        s0.insert(x10);
        s0.insert(x20);

        std::unordered_set<uint32_t> s1;
        s1.insert(x01);
        s1.insert(x11);
        s1.insert(x21);

        std::unordered_set<uint32_t> s;
        s.insert(x00);
        s.insert(x10);
        s.insert(x20);
        s.insert(x01);
        s.insert(x11);
        s.insert(x21);

        s0.erase(x01);
        s0.erase(x11);
        s0.erase(x21);

        s1.erase(x00);
        s1.erase(x10);
        s1.erase(x20);

        s.erase(*s0.begin());
        s.erase(*s1.begin());

        assert(s.size() == 2);
        assert(s0.size() == 1);
        assert(s1.size() == 1);

        auto particle0 = particles[*s0.begin()];
        auto it = s.begin();
        auto particle1 = particles[*it];
        auto particle2 = particles[*++it];
        auto particle3 = particles[*s1.begin()];
        // physics_system.AddDihedral(particle0, particle3, particle1, particle2, 1.f);
        // physics_system.AddDihedral(particle1, particle2, particle0, particle3, 1.f);
        // physics_system.AddSpring(particle0, particle3, 100);
    }
    cloth_.Init();
}
