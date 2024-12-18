#pragma once

#include <Eigen/Eigen>
#include <glm/glm.hpp>

#include <cmath>

namespace math
{
constexpr float EPS = 1e-6;

inline bool IsZero(float value)
{
    return abs(value) < EPS;
}

inline glm::vec3 ToGLM(const Eigen::Vector3f &v)
{
    return {v.x(), v.y(), v.z()};
}

inline Eigen::Vector3f ToEigen(const glm::vec3 &v)
{
    return {v.x, v.y, v.z};
}

int cubic_solver(float a, float b, float c, float d, float *roots);
} // namespace math