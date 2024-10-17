#pragma once

#include <Eigen/Eigen>

class Particle
{
  public:
    Particle(Eigen::Vector3f pos = Eigen::Vector3f(0.f, 0.f, 0.f), Eigen::Vector3f velocity = Eigen::Vector3f(0.f, 0.f, 0.f), float mass_inv = 1.f);
    virtual ~Particle() = default;

    void Update(float dt);
    void AddForce(Eigen::Vector3f force);
    void AddForceDebug(Eigen::Vector3f force);

    Eigen::Vector3f pos_ = Eigen::Vector3f(0.f, 0.f, 0.f);
    Eigen::Vector3f velocity_ = Eigen::Vector3f(0.f, 0.f, 0.f);
    float mass_inv_ = 1.f;

    Eigen::Vector3f force_ = Eigen::Vector3f(0.f, 0.f, 0.f);

    Eigen::Vector3f force_debug_ = Eigen::Vector3f(0.f, 0.f, 0.f);
};