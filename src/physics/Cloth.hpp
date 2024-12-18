#pragma once

#include "Particle.hpp"
#include "Spring.hpp"
#include <Eigen/Eigen>

namespace physics
{

class Cloth
{
  public:
    void Init();
    void Update();
    void UpdateImplicit();
    void UpdateImplicitOpimize();

    void AddParticle(const std::shared_ptr<Particle> &particle);
    void AddSpring(int i, int j, const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1,
                   float stiffness = 1.f);

  public:
    int m_ = 0;
    Eigen::MatrixXf x_;
    Eigen::MatrixXf dx_;
    Eigen::MatrixXf xk_;
    Eigen::MatrixXf v_;
    Eigen::MatrixXf M_;
    Eigen::MatrixXf F_;
    Eigen::MatrixXf f_;
    Eigen::MatrixXf H_;
    Eigen::MatrixXf h_;

    std::vector<std::shared_ptr<Particle>> particles_;
    std::vector<std::shared_ptr<Spring>> spring_;
};
} // namespace physics