#pragma once

#include "Particle.hpp"

#include <memory>

class Spring
{
  public:
    Spring(const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1_,
           float stiffness = 1.f);
    void Update();

    std::shared_ptr<Particle> particle0_ = nullptr, particle1_ = nullptr;
    float rest_length_ = 0.f;
    float stiffness_ = 1.f;
};