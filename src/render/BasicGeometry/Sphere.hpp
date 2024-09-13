#pragma once

#include "render/Mesh/Mesh.hpp"

class Sphere : public Mesh
{
  public:
    void Init(uint32_t lat = 360, uint32_t lng = 180);
};