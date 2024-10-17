#include "Particle.hpp"

#include <utility>
#include <iostream>

using namespace std;
using namespace Eigen;

Particle::Particle(Vector3f pos, Vector3f velocity, float mass_inv)
    : pos_{std::move(pos)}, velocity_{std::move(velocity)}, mass_inv_{mass_inv}
{
}

void Particle::Update(float dt)
{
    velocity_ += dt * mass_inv_ * force_;
    pos_ += dt * velocity_;

    force_ = Eigen::Vector3f(0.f, 0.f, 0.f);
    // std::cout << "debug force" << force_debug_  << std::endl;
    force_debug_ = Eigen::Vector3f(0.f, 0.f, 0.f);
}

void Particle::AddForce(Vector3f force)
{
    force_ += force;
}

void Particle::AddForceDebug(Vector3f force)
{
    force_debug_ += force;
}