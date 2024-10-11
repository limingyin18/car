#pragma once

#include "glm/glm.hpp"

constexpr float EPS = 1e-6;

inline bool IsZero(float value)
{
    return abs(value) < EPS;
}