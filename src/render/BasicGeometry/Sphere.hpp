#pragma once

#include "render/Mesh/Mesh.hpp"

class Sphere : public Mesh
{
  public:
    void Init(uint32_t lat = 360, uint32_t lng = 180, float radius = 1.0f);

    float radius_ = 1.0f;
};