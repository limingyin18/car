#pragma once

#include <cmath>

#include <Eigen/Eigen>
#include <glm/glm.hpp>

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