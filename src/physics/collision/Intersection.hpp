#pragma once

#include <Eigen/Eigen>

namespace physics
{
bool IntersectTriangleTriangle(Eigen::Vector3f const &p1, Eigen::Vector3f const &q1, Eigen::Vector3f const &r1,
                               Eigen::Vector3f const &p2, Eigen::Vector3f const &q2, Eigen::Vector3f const &r2);

bool IntersectPointTriangle(Eigen::Vector3f const &p, Eigen::Vector3f const &x0, Eigen::Vector3f const &x1,
                            Eigen::Vector3f const &x2);

float IntersectEdgePlane(Eigen::Vector3f const &xa, Eigen::Vector3f const &xb, Eigen::Vector3f const &x0,
                         Eigen::Vector3f const &n);

int cubic_solver(float a, float b, float c, float d, float *roots);
} // namespace physics