#include "Triangle.hpp"

using namespace std;

void Triangle::Init()
{
    float vertex_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};
    vector<uint32_t> index = {0, 1, 2};

    std::vector<VertexBase> vertices(3);
    for (uint32_t i = 0; i < 3; ++i)
    {
        vertices[i] = VertexBase{.position_ = {vertex_data[i * 3 + 0],
                                               vertex_data[i * 3 + 1],
                                               vertex_data[i * 3 + 2]},
                                 .normal_ = {0.f, 0.f, 1.0f},
                                 .color_{1.0f, 0.f, 0.f}};
    }
    std::vector<uint32_t> indices(3);
    for (uint32_t i = 0; i < index.size(); ++i)
    {
        indices[i] = index[i];
    }

    mesh_ = make_shared<MeshBase>();
    mesh_->Init(vertices, indices);
}

void Triangle::SetShader(Shader &shader)
{
    mesh_->SetShader(shader);
}

void Triangle::Draw()
{
    mesh_->Draw();
}