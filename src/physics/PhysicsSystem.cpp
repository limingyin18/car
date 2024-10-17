#include "PhysicsSystem.hpp"
#include "physics/Spring.hpp"
#include "tools/Timer.hpp"

#include <iostream>

constexpr float G = 9.8f;
Eigen::Vector3f gravity{0.f, -G, 0.f};

using namespace std;

void PhysicsSystem::Update()
{
    float dt = Timer::GetInstance().GetDeltaT();
    dt = 0.016f;

    for (auto &spring : springs_)
    {
        spring.Update();
    }
    for (auto &dihedral : dihedrals_)
    {
        dihedral.Update();
    }

    for (auto &particle : particles_)
    {
        particle->AddForce(gravity);
    }

    // std::cout << "update" << endl;
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

void PhysicsSystem::AddDihedral(const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1,
                                const std::shared_ptr<Particle> &particle2, const std::shared_ptr<Particle> &particle3,
                                float stiffness)
{
    dihedrals_.emplace_back(particle0, particle1, particle2, particle3, stiffness);
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