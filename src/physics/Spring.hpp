#pragma once

#include "Particle.hpp"

#include <memory>

class Spring
{
  public:
    Spring(const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1_,
           float stiffness = 1.f);
    Spring(int i, int j, const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1_,
           float stiffness = 1.f);
    void Update();
    void UpdateFe(Eigen::Vector3f x0k, Eigen::Vector3f x1k);
    void UpdateHe(Eigen::Vector3f x0k, Eigen::Vector3f x1k);

    std::shared_ptr<Particle> particle0_ = nullptr, particle1_ = nullptr;
    float rest_length_ = 0.f;
    float stiffness_ = 1.f;

    int i = 0;
    int j = 0;
    Eigen::Vector3f fe;
    Eigen::Matrix<float, 3, 3> He;
};