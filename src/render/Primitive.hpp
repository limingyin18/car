#pragma once

#include "Mesh.hpp"
#include "Shader.hpp"

#include <memory>

class Primitive
{
public:
    Primitive() = default;
    ~Primitive();
    virtual void Init() {};

    void Draw(std::shared_ptr<Shader> _shader);

    void SetModel(const glm::mat4 &_model)
    {
        mesh_->SetModel(_model);
    }

    void SetMesh(std::shared_ptr<Mesh> _mesh) { mesh_ = _mesh; };

    Mesh &GetMesh() { return *mesh_; }

protected:
    std::shared_ptr<Mesh> mesh_;
};