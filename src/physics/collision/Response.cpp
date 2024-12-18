#include "Response.hpp"
#include "../Particle.hpp"
#include "Eigen/Core"
#include "math/Geometry.hpp"
#include "physics/collision/CCD.hpp"
#include "physics/collision/Intersection.hpp"
#include "tools/Tool.hpp"
#include <spdlog/spdlog.h>

using namespace physics;
using namespace Eigen;
using namespace math;

float physics::ImpulsePointTriangle(float m0_inv, float m1_inv, float m2_inv, float m3_inv, float w1, float w2,
                                    float w3, float Vrn)
{
    float mass_inv = (m0_inv + w1 * w1 * m1_inv + w2 * w2 * m2_inv + w3 * w3 * m3_inv);
    if (mass_inv == 0.f)
    {
        return 0.f;
    }
    float I = -1.f / mass_inv * Vrn;
    return I;
}

void physics::ApplyImpulsionPointTriangle(Particle &p, Particle &pA, Particle &pB, Particle &pC, Eigen::Vector3f &N,
                                          float w1, float w2, float w3, float I)
{
    float J = (2 * I) / (1 + w1 * w1 + w2 * w2 + w3 * w3);
    p.velocity_ += J * p.mass_inv_ * N;
    pA.velocity_ -= w1 * J * pA.mass_inv_ * N;
    pB.velocity_ -= w2 * J * pB.mass_inv_ * N;
    pC.velocity_ -= w3 * J * pC.mass_inv_ * N;

    // spdlog::debug("J: {}", J);

    bool is_nan = p.velocity_.array().isNaN().any();
    if (is_nan)
    {
        spdlog::debug("p velocity is nan");
    }
}

void physics::ResponsePointTriangle(Particle &p, Particle &pA, Particle &pB, Particle &pC, float dt)
{
    Vector3f pNew = p.pos_ + dt * p.velocity_;
    Vector3f pANew = pA.pos_ + dt * pA.velocity_;
    Vector3f pBNew = pB.pos_ + dt * pB.velocity_;
    Vector3f pCNew = pC.pos_ + dt * pC.velocity_;
    bool is_intersect = IntersectPointTriangle(pNew, pANew, pBNew, pCNew);
    assert(is_intersect);
    std::optional<float> t = FindCollisionTime(p.pos_, pA.pos_, pB.pos_, pC.pos_, p.velocity_ * 0.016f,
                                               pA.velocity_ * 0.016f, pB.velocity_ * 0.016f, pC.velocity_ * 0.016f);
    Vector3f x30 = pCNew - pNew;
    Vector3f x20 = pBNew - pNew;
    Vector3f x10 = pANew - pNew;
    float area = x30.dot(x10.cross(x20));
    float dtt = t.value() * 0.016f;
    Vector3f N = NormalTriangle(pANew, pBNew, pCNew);
    float det = N.dot(p.pos_ - pA.pos_);
    if (det < 0)
    {
        N = -N;
    }

    Vector3f T = Tangent(N);

    std::array<float, 3> barycentric = Barycentric(pNew, pANew, pBNew, pCNew);

    Vector3f d_vec(pNew - barycentric[0] * pANew - barycentric[1] * pBNew - barycentric[2] * pCNew);
    float d_vec_len = d_vec.norm();
    float d = 1.e-2f - d_vec.dot(N);
    assert(d >= 0.f);
    if (d < 0.f)
        return;

    Vector3f Vr((p.velocity_) - barycentric[0] * (pA.velocity_) - barycentric[1] * (pB.velocity_) -
                barycentric[2] * (pC.velocity_));
    float Vrn = Vr.dot(N);
    if (Vrn >= 0)
    {
        return;
    }
    assert(Vrn <= 0);
    float Vrt = Vr.dot(T);
    if (Vrn >= 0.1 * d / dt)
    {
        // return; // things are going to sort themselves out
    }

    float I = ImpulsePointTriangle(p.mass_inv_, pA.mass_inv_, pB.mass_inv_, pC.mass_inv_, barycentric[0],
                                   barycentric[1], barycentric[2], Vrn);
    spdlog::debug("I: {}", I);
    ApplyImpulsionPointTriangle(p, pA, pB, pC, N, barycentric[0], barycentric[1], barycentric[2], I);

    // float It = ImpulsePointTriangle(p.mass_inv_, pA.mass_inv_, pB.mass_inv_, pC.mass_inv_, barycentric[0],
    //                                 barycentric[1], barycentric[2], Vrt);
    // ApplyImpulsionPointTriangle(p, pA, pB, pC, T, barycentric[0], barycentric[1], barycentric[2], It);
}

float physics::ImpluseEdgeEdge(float m0_inv, float m1_inv, float m2_inv, float m3_inv, float a, float b, float Vrn)
{
    float mass_inv = ((1 - a) * (1 - a) * m0_inv + a * a * m1_inv + (1 - b) * (1 - b) * m2_inv + b * b * m3_inv);
    if (mass_inv == 0.f)
    {
        return 0.f;
    }
    float I = -1.f / mass_inv * Vrn;

    return I;
}

void physics::ApplyImpulsionEdgeEdge(Particle &pA, Particle &pB, Particle &pC, Particle &pD, Eigen::Vector3f &N,
                                     float a, float b, float I)
{
    float J = (2 * I) / (a * a + (1 - a) * (1 - a) + b * b + (1 - b) * (1 - b));
    pA.velocity_ += (1 - a) * J * pA.mass_inv_ * N;
    pB.velocity_ += a * J * pB.mass_inv_ * N;
    pC.velocity_ -= (1 - b) * J * pC.mass_inv_ * N;
    pD.velocity_ -= b * J * pD.mass_inv_ * N;
}

void physics::ResponseEdgeEdge(Particle &pA, Particle &pB, Particle &pC, Particle &pD, float dt, float a, float b)
{
    Vector3f pANew = pA.pos_ + dt * pA.velocity_;
    Vector3f pBNew = pB.pos_ + dt * pB.velocity_;
    Vector3f pCNew = pC.pos_ + dt * pC.velocity_;
    Vector3f pDNew = pD.pos_ + dt * pD.velocity_;

    Vector3f N = NormalEdgeEdge(pANew, pBNew, pCNew, pDNew);

    float d = 1.e-2f - ((1 - a) * pANew + a * pBNew - (1 - b) * pCNew - b * pDNew).dot(N);
    if (d < 0.0f)
        return; // outside

    Vector3f Vr((1 - a) * pA.velocity_ + a * pB.velocity_ - (1 - b) * pC.velocity_ - b * pD.velocity_);
    if (N.norm() < 1e-6)
    {
        N = Vr.normalized();
    }
    Vector3f T = Tangent(N);
    float Vrn = Vr.dot(N);
    float Vrt = Vr.dot(T);

    if (Vrn >= 0.1 * d / dt)
    {
        // return; // things are going to sort themselves out
    }

    float I = ImpluseEdgeEdge(pA.mass_inv_, pB.mass_inv_, pC.mass_inv_, pD.mass_inv_, a, b, Vrn);
    ApplyImpulsionEdgeEdge(pA, pB, pC, pD, N, a, b, I);

    float It = ImpluseEdgeEdge(pA.mass_inv_, pB.mass_inv_, pC.mass_inv_, pD.mass_inv_, a, b, Vrt);
    ApplyImpulsionEdgeEdge(pA, pB, pC, pD, T, a, b, It);
}

void physics::RepulsionPointTriangle(Particle &p, Particle &pA, Particle &pB, Particle &pC, float dt)
{
    Vector3f N = NormalTriangle(pA.pos_, pB.pos_, pC.pos_);
    Vector3f T = Tangent(N);

    std::array<float, 3> barycentric = Barycentric(p.pos_, pA.pos_, pB.pos_, pC.pos_);

    float d = 1.e-2f - (p.pos_ - barycentric[0] * pA.pos_ - barycentric[1] * pB.pos_ - barycentric[2] * pC.pos_).dot(N);
    if (d < 0.f)
        return;

    Vector3f Vr((p.velocity_) - barycentric[0] * (pA.velocity_) - barycentric[1] * (pB.velocity_) -
                barycentric[2] * (pC.velocity_));
    float Vrn = Vr.dot(N);
    float Vrt = Vr.dot(T);
    if (Vrn >= 0.1 * d / dt)
    {
        return; // things are going to sort themselves out
    }

    float I = RepulsionImpulse(p.mass_inv_, Vrn, d, dt);
    ApplyImpulsionPointTriangle(p, pA, pB, pC, N, barycentric[0], barycentric[1], barycentric[2], I);
}

float physics::RepulsionImpulse(float m0_inv, float Vrn, float d, float dt)
{
    float k = 150.0f; // todo: LOL
    float I = m0_inv == 0.f ? -dt * k * d : -fmin(dt * k * d, (1.f / m0_inv) * (0.1f * d / dt - Vrn));
    return I;
}

void physics::RepulsionEdgeEdge(Particle &pA, Particle &pB, Particle &pC, Particle &pD, float dt, float a, float b)
{
    Vector3f N = NormalEdgeEdge(pA.pos_, pB.pos_, pC.pos_, pD.pos_);

    float d = 1.e-2f - ((1 - a) * pA.pos_ + a * pB.pos_ - (1 - b) * pC.pos_ - b * pC.pos_).dot(N);
    if (d < 0.0f)
        return; // outside

    Vector3f Vr((1 - a) * pA.velocity_ + a * pB.velocity_ - (1 - b) * pC.velocity_ - b * pD.velocity_);
    if (N.norm() < 1e-6)
    {
        N = Vr.normalized();
    }
    Vector3f T = Tangent(N);
    float Vrn = Vr.dot(N);
    float Vrt = Vr.dot(T);

    if (Vrn >= 0.1 * d / dt)
    {
        return; // things are going to sort themselves out
    }

    float I = RepulsionImpulse(pA.mass_inv_, Vrn, d, dt);
    ApplyImpulsionEdgeEdge(pA, pB, pC, pD, N, a, b, I);
}