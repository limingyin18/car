#pragma once

#include <array>
#include <memory>

#include "Particle.hpp"

/**
 * @brief 二面角
 *
 */
class Dihedral
{
  public:
    Dihedral(const std::shared_ptr<Particle> &p0, const std::shared_ptr<Particle> &p1,
             const std::shared_ptr<Particle> &p2, const std::shared_ptr<Particle> &p3, float stiffness = 1.f);
    virtual ~Dihedral() = default;
    // Dihedral(const Dihedral &rhs) = delete;
    // Dihedral(Dihedral &&rhs) = delete;
    // Dihedral &operator=(const Dihedral &rhs) = delete;
    // Dihedral &operator=(Dihedral &&rhs) = delete;

    void Update();

  private:
    std::array<std::shared_ptr<Particle>, 4> particles_;
    float stiffness_ = 1.f;
};