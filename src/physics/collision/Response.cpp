#include "Response.hpp"
#include "../Particle.hpp"
#include "Eigen/Core"
#include "math/Geometry.hpp"

using namespace physics;
using namespace Eigen;
using namespace math;

float physics::ImpulsePointTriangle(float m0_inv, float m1_inv, float m2_inv, float m3_inv, float w1, float w2,
                                    float w3, float Vrn)
{
    float I = -1.f / (m0_inv + w1 * w1 * m1_inv + w2 * w2 * m2_inv + w3 * w3 * m3_inv) * Vrn;
    return I;
}

void physics::ApplyImpulsionPointTriangle(Particle &p, Particle &pA, Particle &pB, Particle &pC, Eigen::Vector3f &N,
                                          float w1, float w2, float w3, float I)
{
    float J = (2 * I) / (1 + w1 * w1 + w2 * w2 + w3 * w3);
    p.velocity_ -= J * p.mass_inv_ * N;
    pA.velocity_ += w1 * J * pA.mass_inv_ * N;
    pB.velocity_ += w2 * J / pB.mass_inv_ * N;
    pC.velocity_ += w3 * J / pC.mass_inv_ * N;
}

void physics::ResponsePointTriangle(Particle &p, Particle &pA, Particle &pB, Particle &pC, float dt)
{
    Vector3f pNew = p.pos_ + dt * p.velocity_;
    Vector3f pANew = pA.pos_ + dt * pA.velocity_;
    Vector3f pBNew = pB.pos_ + dt * pB.velocity_;
    Vector3f pCNew = pC.pos_ + dt * pC.velocity_;

    Vector3f N = NormalTriangle(pANew, pBNew, pCNew);
    Vector3f T = Tangent(N);
    std::array<float, 3> barycentric = Barycentric(pNew, pANew, pBNew, pCNew);

    float d = 1.e-2f - (pNew - barycentric[0] * pANew - barycentric[1] * pBNew - barycentric[2] * pCNew).dot(N);
    if (d < 0.f)
        return;

    Vector3f Vr((pNew - p.velocity_) - barycentric[0] * (pANew - pA.velocity_) -
                barycentric[1] * (pBNew - pB.velocity_) - barycentric[2] * (pCNew - pC.velocity_));
    float Vrn = Vr.dot(N);
    float Vrt = Vr.dot(T);
    if (Vrn >= 0.1 * d / dt)
    {
        return; // things are going to sort themselves out
    }

    float I = ImpulsePointTriangle(p.mass_inv_, pA.mass_inv_, pB.mass_inv_, pC.mass_inv_, barycentric[0],
                                   barycentric[1], barycentric[2], Vrn);
    ApplyImpulsionPointTriangle(p, pA, pB, pC, N, barycentric[0], barycentric[1], barycentric[2], I);
}

float physics::ImpluseEdgeEdge(float m0_inv, float m1_inv, float m2_inv, float m3_inv, float a, float b, float Vrn)
{
    float I = -1.f / ((1 - a) * (1 - a) * m0_inv + a * a * m1_inv + (1 - b) * (1 - b) * m2_inv + b * b * m3_inv) * Vrn;
    return I;
}

void physics::ApplyImpulsionEdgeEdge(Particle &pA, Particle &pB, Particle &pC, Particle &pD, Eigen::Vector3f &N,
                                     float a, float b, float I)
{
    float J = (2 * I) / (a * a + (1 - a) * (1 - a) + b * b + (1 - b) * (1 - b));
    pA.velocity_ += (1 - a) * J * pA.mass_inv_ * N;
    pB.velocity_ += a * J * pB.mass_inv_ * N;
    pC.velocity_ -= (1 - b) * J / pC.mass_inv_ * N;
    pD.velocity_ -= b * J / pD.mass_inv_ * N;
}

void physics::ResponseEdgeEdge(Particle &pA, Particle &pB, Particle &pC, Particle &pD, float dt, float a, float b)
{
    Vector3f pANew = pA.pos_ + dt * pA.velocity_;
    Vector3f pBNew = pB.pos_ + dt * pB.velocity_;
    Vector3f pCNew = pC.pos_ + dt * pC.velocity_;
    Vector3f pDNew = pD.pos_ + dt * pD.velocity_;

    Vector3f N = NormalEdgeEdge(pANew, pBNew, pCNew, pDNew);
    Vector3f T = Tangent(N);
    float d = 1.e-2f - ((1 - a) * pANew + a * pBNew - (1 - b) * pCNew - b * pDNew).dot(N);
    if (d < 0.0f)
        return; // outside

    Vector3f Vr((1 - a) * pA.velocity_ + a * pB.velocity_ - (1 - b) * pC.velocity_ - b * pD.velocity_);
    float Vrn = Vr.dot(N);
    float Vrt = Vr.dot(T);

    if (Vrn >= 0.1 * d / dt)
    {
        return; // things are going to sort themselves out
    }

    float I = ImpluseEdgeEdge(pA.mass_inv_, pB.mass_inv_, pC.mass_inv_, pD.mass_inv_, a, b, Vrn);
    ApplyImpulsionEdgeEdge(pA, pB, pC, pD, N, a, b, I);
}