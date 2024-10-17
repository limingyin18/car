#include "Spring.hpp"
#include "Math.hpp"
#include "glm/geometric.hpp"
#include "physics/Particle.hpp"
#include <cassert>

using namespace std;
using namespace Eigen;

Spring::Spring(const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1_, float stiffness)
    : particle0_(particle0), particle1_(particle1_), stiffness_(stiffness)
{
    rest_length_ = (particle0_->pos_ - particle1_->pos_).norm();
}

void Spring::Update()
{
    Vector3f delta_x = particle0_->pos_ - particle1_->pos_;
    float delta_x_len = delta_x.norm();
    if (IsZero(delta_x_len))
    {
        return;
    }
    Vector3f delta_x_dir = delta_x.normalized();

    Vector3f force = -stiffness_ * (delta_x_len - rest_length_) * delta_x_dir;

    particle0_->AddForce(force);
    particle1_->AddForce(-force);
}