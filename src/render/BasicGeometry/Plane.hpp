#pragma once

#include "render/Mesh/MeshTBN.hpp"
#include <cstdint>

class Plane : public MeshTBN
{
  public:
    void Init(uint32_t n = 2);
};