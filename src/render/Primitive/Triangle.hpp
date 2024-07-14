#pragma once

#include "MeshBase.hpp"
#include "render/Shader/Shader.hpp"

#include <memory>

class Triangle
{
public:
    void Init();
    void SetShader(Shader &shader);
    void Draw();

private:
    std::shared_ptr<MeshBase> mesh_;
};