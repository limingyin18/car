#include "PhysicsSystem.hpp"
#include "physics/Spring.hpp"
#include "tools/Timer.hpp"
#include <cstdint>
#include <iostream>

constexpr float G = 9.8f;
constexpr glm::vec3 gravity = glm::vec3(0.f, -G, 0.f);

using namespace std;

void PhysicsSystem::Update()
{
    float dt = Timer::GetInstance().GetDeltaT();
    dt = 0.016f;

    for (auto &spring : springs_)
    {
        spring.Update();
    }

    for (auto &particle : particles_)
    {
        particle->AddForce(gravity);
    }

    for (auto &particle : particles_)
    {
        particle->Update(dt);
    }
}

void PhysicsSystem::AddSpring(const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1,
                              float stiffness)
{
    springs_.emplace_back(particle0, particle1, stiffness);
}

void PhysicsSystem::AddParticle(const std::shared_ptr<Particle> &particle)
{
    particles_.push_back(particle);
}

void PhysicsSystem::Clear()
{
    particles_.clear();
    springs_.clear();
}