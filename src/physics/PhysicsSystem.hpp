#pragma once

#include "Particle.hpp"
#include "Spring.hpp"

#include "tools/Singleton.hpp"
#include "tools/System.hpp"

#include <vector>

class PhysicsSystem : public Singleton<PhysicsSystem>, public System
{
    friend class Singleton<PhysicsSystem>;

  public:
    PhysicsSystem() = default;
    virtual ~PhysicsSystem() = default;

    void Update() override;
    void AddSpring(const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1,
                   float stiffness = 1.f);
    void AddParticle(const std::shared_ptr<Particle> &particle);
    void Clear();

    std::vector<Spring> springs_;
    std::vector<std::shared_ptr<Particle>> particles_;
};