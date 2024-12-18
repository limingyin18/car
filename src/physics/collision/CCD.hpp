#pragma once

#include "physics/Particle.hpp"
#include <Eigen/Eigen>

namespace physics
{

/**
 * @brief check if point p intersects with triangle x0, x1, x2
 * assumes t = 1, v *= dt
 *
 * @param x0
 * @param x1
 * @param x2
 * @param x3
 * @param v0
 * @param v1
 * @param v2
 * @param v3
 * @return std::optional<float>
 */
std::optional<float> CCDPointTriangle(Eigen::Vector3f const &x0, Eigen::Vector3f const &x1, Eigen::Vector3f const &x2,
                                      Eigen::Vector3f const &x3, Eigen::Vector3f const &v0, Eigen::Vector3f const &v1,
                                      Eigen::Vector3f const &v2, Eigen::Vector3f const &v3);

std::optional<std::pair<float, std::array<float, 2>>> CCDEdgeEdge(Eigen::Vector3f const &x0, Eigen::Vector3f const &x1, Eigen::Vector3f const &x2,
                                 Eigen::Vector3f const &x3, Eigen::Vector3f const &v0, Eigen::Vector3f const &v1,
                                 Eigen::Vector3f const &v2, Eigen::Vector3f const &v3);

std::optional<float> FindCollisionTime(Eigen::Vector3f const &x0, Eigen::Vector3f const &x1, Eigen::Vector3f const &x2,
                                       Eigen::Vector3f const &x3, Eigen::Vector3f const &v0, Eigen::Vector3f const &v1,
                                       Eigen::Vector3f const &v2, Eigen::Vector3f const &v3);

std::optional<float> CCDTriangleTriangle(Particle &p0, Particle &p1, Particle &p2, Particle &p3, Particle &p4,
                                         Particle &p5, float dt);

std::optional<float> CCDTriangleTriangleCheck(Particle &p0, Particle &p1, Particle &p2, Particle &p3, Particle &p4,
                                         Particle &p5, float dt);
} // namespace physics