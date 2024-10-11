#include "Particle.hpp"
#include "Math.hpp"
#include <iostream>

using namespace std;

Particle::Particle(glm::vec3 pos, glm::vec3 velocity, float mass_inv)
    : pos_{pos}, velocity_{velocity}, mass_inv_{mass_inv}
{
}

void Particle::Update(float dt)
{
    velocity_ += dt * mass_inv_ * force_;
    pos_ += dt * velocity_;

    force_ = glm::vec3(0.f);
}

void Particle::AddForce(glm::vec3 force)
{
    force_ += force;
}