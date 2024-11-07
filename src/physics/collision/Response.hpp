#pragma once

#include <Eigen/Eigen>

class Particle;

namespace physics
{
void ResponsePointTriangle(Particle &p, Particle &pA, Particle &pB, Particle &pC, float dt);

float ImpulsePointTriangle(float m0_inv, float m1_inv, float m2_inv, float m3_inv, float w1, float w2, float w3,
                           float Vrn);

void ApplyImpulsionPointTriangle(Particle &p, Particle &pA, Particle &pB, Particle &pC, Eigen::Vector3f &N, float w1,
                                 float w2, float w3, float I);

float ImpluseEdgeEdge(float m0_inv, float m1_inv, float m2_inv, float m3_inv, float a, float b, float Vrn);

void ApplyImpulsionEdgeEdge(Particle &pA, Particle &pB, Particle &pC, Particle &pD, Eigen::Vector3f &N, float a,
                            float b, float I);

void ResponseEdgeEdge(Particle &pA, Particle &pB, Particle &pC, Particle &pD, float dt, float a, float b);
} // namespace physics