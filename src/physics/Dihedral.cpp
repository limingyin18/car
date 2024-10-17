#include "Dihedral.hpp"

#include "Particle.hpp"

#include <cmath>
#include <numbers>
#include <iostream>

using namespace std;
using namespace Eigen;

Dihedral::Dihedral(const std::shared_ptr<Particle> &p0, const std::shared_ptr<Particle> &p1,
                   const std::shared_ptr<Particle> &p2, const std::shared_ptr<Particle> &p3, float stiffness)
{
    particles_[0] = p0;
    particles_[1] = p1;
    particles_[2] = p2;
    particles_[3] = p3;

    stiffness_ = stiffness;
}

void Dihedral::Update()
{
    Vector3f x1 = particles_[0]->pos_;
    Vector3f x2 = particles_[1]->pos_;
    Vector3f x3 = particles_[2]->pos_;
    Vector3f x4 = particles_[3]->pos_;

    Vector3f e1 = x1 - x3;
    Vector3f e2 = x1 - x4;
    Vector3f e3 = x2 - x4;
    Vector3f e4 = x2 - x3;
    Vector3f e = x4 - x3;
    float l = e.norm();

    Vector3f n1 = e1.cross(e2);
    float l1 = n1.squaredNorm();
    Vector3f n2 = e3.cross(e4);
    float l2 = n2.squaredNorm();

    Vector3f u1 = l * n1 / l1;
    Vector3f u2 = l * n2 / l2;
    Vector3f u3 = e2.dot(e) / l * n1 / l1 + e3.dot(e) / l * n2 / l2;
    Vector3f u4 = -e1.dot(e) / l * n1 / l1 - e4.dot(e) / l * n2 / l2;

    float dot_product = n1.dot(n2);
    float norm_v1 = n1.norm();
    float norm_v2 = n2.norm();

    float cos_theta = dot_product / (norm_v1 * norm_v2);
    cos_theta = std::clamp(cos_theta, -1.0f, 1.0f);
    float angle = acos(cos_theta);

    float f = stiffness_ * sin((float(numbers::pi) - angle) * 0.5f) * l * l / (norm_v1 + norm_v2);
    // std::cout << f << endl;

    Vector3f f1 = f * u1;
    Vector3f f2 = f * u2;
    Vector3f f3 = f * u3;
    Vector3f f4 = f * u4;

    particles_[0]->AddForce(f1);
    particles_[1]->AddForce(f2);
    particles_[2]->AddForce(f3);
    particles_[3]->AddForce(f4);

    particles_[0]->AddForceDebug(f1);
    particles_[1]->AddForceDebug(f2);
    particles_[2]->AddForceDebug(f3);
    particles_[3]->AddForceDebug(f4);
}