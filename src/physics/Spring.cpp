#include "Spring.hpp"
#include "Math.hpp"
#include "glm/geometric.hpp"
#include "physics/Particle.hpp"
#include <cassert>
#include <iostream>

using namespace std;

Spring::Spring(const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1_, float stiffness)
    : particle0_(particle0), particle1_(particle1_)
{
    rest_length_ = glm::distance(particle0_->pos_, particle1_->pos_);
}

void Spring::Update()
{
    glm::vec3 delta_x = particle0_->pos_ - particle1_->pos_;
    float delta_x_len = glm::length(delta_x);
    if (IsZero(delta_x_len))
    {
        return;
    }
    glm::vec3 delta_x_dir = glm::normalize(delta_x);

    glm::vec3 force = -stiffness_ * (delta_x_len - rest_length_) * delta_x_dir;

    particle0_->AddForce(force);
    particle1_->AddForce(-force);
}