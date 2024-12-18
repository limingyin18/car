
#include "SceneClothCube.hpp"

#include "BasicActor/Cloth.hpp"
#include "game/Scene.hpp"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/string_cast.hpp"
#include "math/Math.hpp"
#include "physics/Particle.hpp"
#include "physics/collision/CCD.hpp"
#include "physics/collision/Intersection.hpp"
#include "physics/collision/Response.hpp"
#include "render/BasicGeometry/Cube.hpp"
#include "render/BasicGeometry/Plane.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"
#include "tools/Timer.hpp"
#include "tools/Tool.hpp"
#include <cstdint>
#include <glm/gtx/matrix_decompose.hpp>

#include <cstddef>
#include <memory>

#include "math/Geometry.hpp"
#include <Eigen/Eigen>

using namespace std;
using namespace math;
using namespace Eigen;

void SceneClothCube::Init()
{
    InitCloth();
    InitCube();
    Scene::Init();
}

void SceneClothCube::InitCloth()
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

void SceneClothCube::InitCube()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    cube_ = make_shared<Actor>();
    cube_->SetShader(shaders_map["phong"]);

    auto material = std::make_shared<Material>();
    Texture texture;
    texture.id = render.GetTexture("default_blue");
    material->SetTexture(0, texture);
    material->SetFloat("ambientStrength", 0.00f);
    material->SetFloat("specularStrength", 0.5f);
    material->SetFloat("specularPow", 255.f);

    cube_->SetMaterial(material);

    auto mesh = make_shared<Cube>();
    mesh->SetDynamic();
    mesh->Init(2u);
    cube_->AddMesh(mesh);

    MeshAdjacentList(*mesh);

    auto transform = glm::mat4(1.f);
    cube_->SetTransform(transform);

    actors_.push_back(cube_);
}

void SceneClothCube::Update()
{
    Scene::Update();
}

void SceneClothCube::UpdatePhysics()
{
    Scene::UpdatePhysics();
    // ApplyReplusionForce();
    CollideClothCube();
    UpdateClothPosition();
}

void SceneClothCube::UpdateClothPosition()
{
    auto &particles = cloth_->cloth_.particles_;

    for (auto &particle : particles)
    {
        particle->pos_ += particle->velocity_ * 0.016f;
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

void SceneClothCube::CollideClothCube()
{
    auto &particles = cloth_->cloth_.particles_;

    // ccd
    {
        auto mesh = dynamic_pointer_cast<MeshTBN>(cloth_->GetMeshes()[0]);
        auto &vertices = mesh->GetVertices();
        auto &indices = mesh->GetIndices();

        auto mesh_cube = dynamic_pointer_cast<Cube>(cube_->GetMeshes()[0]);
        auto &vertices_cube = mesh_cube->GetVertices();
        auto &indices_cube = mesh_cube->GetIndices();

        for (uint32_t i = 0; i < indices.size(); i += 3)
        {
            uint32_t index0 = indices[i + 0];
            uint32_t index1 = indices[i + 1];
            uint32_t index2 = indices[i + 2];
            auto &p0 = *cloth_->cloth_.particles_[index0];
            auto &p1 = *cloth_->cloth_.particles_[index1];
            auto &p2 = *cloth_->cloth_.particles_[index2];

            for (uint32_t j = 0; j < indices_cube.size(); j += 3)
            {
                uint32_t index0_sphere = indices_cube[j + 0];
                uint32_t index1_sphere = indices_cube[j + 1];
                uint32_t index2_sphere = indices_cube[j + 2];
                auto p3 =
                    Particle(ToEigen(vertices_cube[index0_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);
                auto p4 =
                    Particle(ToEigen(vertices_cube[index1_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);
                auto p5 =
                    Particle(ToEigen(vertices_cube[index2_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);

                auto t = physics::CCDTriangleTriangle(p0, p1, p2, p3, p4, p5, 0.016f);
                auto t_check = physics::CCDTriangleTriangleCheck(p0, p1, p2, p3, p4, p5, 0.016f);
                assert(t_check.has_value() == false);
            }
        }
    }
    // // sdf
    // {
    //     for (auto &particle : particles)
    //     {
    //         auto dist = sdf(particle->pos_);

    //         if (dist.first < 0.f)
    //         {
    //             Eigen::Vector3f A = particle->pos_ - dist.first * dist.second;
    //             particle->velocity_ = (A - particle->pos_) / 0.016f;
    //             // particle->mass_inv_ = 0.f;
    //         }
    //     }
    // }
}

void SceneClothCube::ApplyReplusionForce()
{
    auto &particles = cloth_->cloth_.particles_;
    float dt = Timer::GetInstance().GetPhysicsFixedDeltaTime();

    {
        auto mesh = dynamic_pointer_cast<MeshTBN>(cloth_->GetMeshes()[0]);
        auto &vertices = mesh->GetVertices();
        auto &indices = mesh->GetIndices();

        auto mesh_cube = dynamic_pointer_cast<Cube>(cube_->GetMeshes()[0]);
        auto &vertices_cube = mesh_cube->GetVertices();
        auto &indices_cube = mesh_cube->GetIndices();

        for (auto &particle : particles)
        {
            for (uint32_t j = 0; j < indices_cube.size(); j += 3)
            {
                uint32_t index0_sphere = indices_cube[j + 0];
                uint32_t index1_sphere = indices_cube[j + 1];
                uint32_t index2_sphere = indices_cube[j + 2];
                auto p3 =
                    Particle(ToEigen(vertices_cube[index0_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);
                auto p4 =
                    Particle(ToEigen(vertices_cube[index1_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);
                auto p5 =
                    Particle(ToEigen(vertices_cube[index2_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);

                if (physics::TestPointTriangle(particle->pos_, p3.pos_, p4.pos_, p5.pos_))
                {
                    // spdlog::debug("repulsion");
                    physics::RepulsionPointTriangle(*particle, p3, p4, p5, dt);
                }
            }
        }

        for (uint32_t i = 0; i < indices.size(); i += 3)
        {
            uint32_t index0 = indices[i + 0];
            uint32_t index1 = indices[i + 1];
            uint32_t index2 = indices[i + 2];
            auto &p0 = *cloth_->cloth_.particles_[index0];
            auto &p1 = *cloth_->cloth_.particles_[index1];
            auto &p2 = *cloth_->cloth_.particles_[index2];

            for (uint32_t j = 0; j < indices_cube.size(); j += 3)
            {
                uint32_t index0_sphere = indices_cube[j + 0];
                uint32_t index1_sphere = indices_cube[j + 1];
                uint32_t index2_sphere = indices_cube[j + 2];
                auto p3 =
                    Particle(ToEigen(vertices_cube[index0_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);
                auto p4 =
                    Particle(ToEigen(vertices_cube[index1_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);
                auto p5 =
                    Particle(ToEigen(vertices_cube[index2_sphere].position_), Eigen::Vector3f(0.f, 0.f, 0.f), 0.f);

                auto ReEdgeEdge = [dt](Particle &p0, Particle &p1, Particle &p2, Particle &p3) {
                    auto st = physics::IntersectEdgeEdge(p0.pos_, p1.pos_, p2.pos_, p3.pos_);
                    if (st.has_value())
                    {
                        // spdlog::debug("repulsion");
                        physics::RepulsionEdgeEdge(p0, p1, p2, p3, dt, st.value()[0], st.value()[1]);
                    }
                };

                ReEdgeEdge(p0, p1, p3, p4);
                ReEdgeEdge(p0, p1, p3, p5);
                ReEdgeEdge(p0, p1, p4, p5);
                ReEdgeEdge(p0, p2, p3, p4);
                ReEdgeEdge(p0, p2, p3, p5);
                ReEdgeEdge(p0, p2, p4, p5);
                ReEdgeEdge(p1, p2, p3, p4);
                ReEdgeEdge(p1, p2, p3, p5);
                ReEdgeEdge(p1, p2, p4, p5);
            }
        }
    }
}

void SceneClothCube::MeshAdjacentList(Mesh &mesh)
{
    auto &vertices = mesh.GetVertices();
    auto &indices = mesh.GetIndices();

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

    std::vector<Edge> edges;
    // std::vector<std::pair<uint32_t, uint32_t>> adjacency_list_;
    // std::map<Edge, std::pair<uint32_t, uint32_t>> edge_to_triangle;
    for (size_t i = 0; i < triples.size(); ++i)
    {
        auto triple = triples[i];
        Edge edge;
        edge.vertex_id_start_ = triple.vertex_id_start_;
        edge.vertex_id_end_ = triple.vertex_id_end_;
        if (edges.empty())
        {
            edges.push_back(edge);
            continue;
        }
        if (edges.back().vertex_id_start_ == edge.vertex_id_start_ &&
            edges.back().vertex_id_end_ == edge.vertex_id_end_)
        {
            adjacency_list_.emplace_back(triples[i - 1].triangle_id_, triple.triangle_id_);
            edge_to_triangle_[edge] = {triples[i - 1].triangle_id_, triple.triangle_id_};
            continue;
        }

        edges.push_back(edge);
    }
}

std::pair<float, Eigen::Vector3f> SceneClothCube::sdf(const Eigen::Vector3f &pos)
{
    auto mesh = dynamic_pointer_cast<Mesh>(cube_->GetMeshes()[0]);
    auto &vertices = mesh->GetVertices();
    auto &indices = mesh->GetIndices();

    std::vector<bool> is_visited(indices.size() / 3, false);

    uint32_t triangle_id = 0;

    uint32_t index0 = indices[triangle_id * 3 + 0];
    uint32_t index1 = indices[triangle_id * 3 + 1];
    uint32_t index2 = indices[triangle_id * 3 + 2];

    Vector3f v0 = ToEigen(vertices[index0].position_);
    Vector3f v1 = ToEigen(vertices[index1].position_);
    Vector3f v2 = ToEigen(vertices[index2].position_);

    Vector3f c = (v0 + v1 + v2) / 3.f;
    while (1)
    {
        is_visited[triangle_id] = true;
        index0 = indices[triangle_id * 3 + 0];
        index1 = indices[triangle_id * 3 + 1];
        index2 = indices[triangle_id * 3 + 2];

        v0 = ToEigen(vertices[index0].position_);
        v1 = ToEigen(vertices[index1].position_);
        v2 = ToEigen(vertices[index2].position_);

        Vector3f n = NormalTriangle(v0, v1, v2);
        Vector3f r = ClosestPtPointPlane(pos, c, n);

        auto st = physics::IntersectEdgeEdge(r, c, v0, v1);
        uint32_t edge_vertex_index_0 = index0;
        uint32_t edge_vertex_index_1 = index1;
        if (!st.has_value())
        {
            edge_vertex_index_0 = index0;
            edge_vertex_index_1 = index2;
            st = physics::IntersectEdgeEdge(r, c, v0, v2);
            if (!st.has_value())
            {
                edge_vertex_index_0 = index1;
                edge_vertex_index_1 = index2;
                st = physics::IntersectEdgeEdge(r, c, v1, v2);
                if (!st.has_value())
                {
                    float dist = (pos - r).dot(n);
                    return {dist, n};
                }
                else
                {
                    r = r + st.value()[0] * (c - r);
                }
            }
            else
            {
                r = r + st.value()[0] * (c - r);
            }
        }
        else
        {
            r = r + st.value()[0] * (c - r);
        }

        if (edge_vertex_index_0 > edge_vertex_index_1)
        {
            std::swap(edge_vertex_index_0, edge_vertex_index_1);
        }
        Edge e0(edge_vertex_index_0, edge_vertex_index_1);
        auto it = edge_to_triangle_.find(e0);
        if (it != edge_to_triangle_.end())
        {
            auto &t = it->second;
            uint32_t triangle_id_0 = t.first;
            uint32_t triangle_id_1 = t.second;

            if (triangle_id_0 == triangle_id)
            {
                if (is_visited[triangle_id_1] != true)
                {
                    triangle_id = triangle_id_1;
                    c = r;
                }
                else
                {
                    float dist = (pos - r).dot(n);
                    return {dist, n};
                }
            }
            else
            {
                if (is_visited[triangle_id_0] != true)
                {
                    triangle_id = triangle_id_0;
                    c = r;
                }
                else
                {
                    float dist = (pos - r).dot(n);
                    return {dist, n};
                }
            }
        }
        else
        {
            float dist = (pos - r).dot(n);
            return {dist, n};
        }
    }
}