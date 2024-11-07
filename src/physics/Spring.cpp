#include "Spring.hpp"
#include "Eigen/Core"
#include "math/Math.hpp"
#include "glm/geometric.hpp"
#include "physics/Particle.hpp"
#include <cassert>

#include "tools/Timer.hpp"

using namespace std;
using namespace Eigen;
using namespace math;

Spring::Spring(const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1_, float stiffness)
    : particle0_(particle0), particle1_(particle1_), stiffness_(stiffness)
{
    rest_length_ = (particle0_->pos_ - particle1_->pos_).norm();
}

Spring::Spring(int i, int j, const std::shared_ptr<Particle> &particle0, const std::shared_ptr<Particle> &particle1_,
               float stiffness)
    : i(i), j(j), particle0_(particle0), particle1_(particle1_), stiffness_(stiffness)
{
    rest_length_ = (particle0_->pos_ - particle1_->pos_).norm();
}

// void Spring::Update()
// {
//     Vector3f delta_x = particle0_->pos_ - particle1_->pos_;
//     float delta_x_len = delta_x.norm();
//     if (IsZero(delta_x_len))
//     {
//         return;
//     }
//     Vector3f delta_x_dir = delta_x.normalized();

//     Vector3f force = -stiffness_ * (delta_x_len - rest_length_) * delta_x_dir;

//     particle0_->AddForce(force);
//     particle1_->AddForce(-force);
// }

// void Spring::Update()
// {
//     float dt = Timer::GetInstance().GetDeltaT();
//     float dt2 = dt * dt;

//     Matrix<float, 6, 1> x;
//     x.block<3, 1>(0, 0) = particle0_->pos_;
//     x.block<3, 1>(3, 0) = particle1_->pos_;

//     Matrix<float, 6, 1> xk;
//     xk.block<3, 1>(0, 0) = particle0_->pos_;
//     xk.block<3, 1>(3, 0) = particle1_->pos_;

//     Matrix<float, 6, 1> v;
//     v.block<3, 1>(0, 0) = particle0_->velocity_;
//     v.block<3, 1>(3, 0) = particle1_->velocity_;

//     Matrix<float, 6, 1> mdiag;
//     mdiag.block<3, 1>(0, 0) = Vector3f{}.setOnes() * 1.f / particle0_->mass_inv_;
//     mdiag.block<3, 1>(3, 0) = Vector3f{}.setOnes() * 1.f / particle1_->mass_inv_;
//     Matrix<float, 6, 6> M = mdiag.asDiagonal();

//     for (int k = 0; k < 10; k++)
//     {
//         Vector3f x0k = xk.block<3, 1>(0, 0);
//         Vector3f x1k = xk.block<3, 1>(3, 0);
//         Vector3f x01 = x0k - x1k;
//         float x01_len = x01.norm();
//         float x01_len2 = x01_len * x01_len;
//         Matrix<float, 3, 3> xxt = x01 * x01.transpose() / x01_len2;
//         Matrix<float, 3, 3> He =
//             stiffness_ * xxt + stiffness_ * (1.f - rest_length_ / x01_len) * (Matrix<float, 3, 3>::Identity() - xxt);
//         Matrix<float, 6, 6> h;
//         h.block<3, 3>(0, 0) = He;
//         h.block<3, 3>(0, 3) = -He;
//         h.block<3, 3>(3, 0) = -He;
//         h.block<3, 3>(3, 3) = He;

//         Vector3f x01_dir = x01.normalized();
//         Vector3f fe = -stiffness_ * (x01_len - rest_length_) * x01_dir;
//         Matrix<float, 6, 1> f;
//         f.block<3, 1>(0, 0) = fe;
//         f.block<3, 1>(3, 0) = -fe;

//         Matrix<float, 6, 1> F = 1.f / dt2 * M * (xk - x - dt * v) - f;
//         Matrix<float, 6, 6> H = 1.f / dt2 * M - h;
//         Matrix<float, 6, 1> dx = -H.inverse() * F;
//         xk += dx;
//     }

//     Vector3f x0n = xk.block<3, 1>(0, 0);
//     Vector3f x1n = xk.block<3, 1>(3, 0);
//     particle0_->velocity_ = (x0n - particle0_->pos_) / dt;
//     particle0_->pos_ = xk.block<3, 1>(0, 0);

//     particle1_->velocity_ = (x1n - particle1_->pos_) / dt;
//     particle1_->pos_ = xk.block<3, 1>(3, 0);
// }

void Spring::Update()
{
    float dt = Timer::GetInstance().GetDeltaT();
    float dt2 = dt * dt;

    Matrix<float, 6, 1> x;
    x.block<3, 1>(0, 0) = particle0_->pos_;
    x.block<3, 1>(3, 0) = particle1_->pos_;

    Matrix<float, 6, 1> xk;
    xk.block<3, 1>(0, 0) = particle0_->pos_;
    xk.block<3, 1>(3, 0) = particle1_->pos_;

    Matrix<float, 6, 1> v;
    v.block<3, 1>(0, 0) = particle0_->velocity_;
    v.block<3, 1>(3, 0) = particle1_->velocity_;

    Matrix<float, 6, 1> mdiag;
    mdiag.block<3, 1>(0, 0) = Vector3f{}.setOnes() * 1.f / particle0_->mass_inv_;
    mdiag.block<3, 1>(3, 0) = Vector3f{}.setOnes() * 1.f / particle1_->mass_inv_;
    Matrix<float, 6, 6> M = mdiag.asDiagonal();

    for (int k = 0; k < 10; k++)
    {
        Vector3f x0k = xk.block<3, 1>(0, 0);
        Vector3f x1k = xk.block<3, 1>(3, 0);
        Vector3f x01 = x0k - x1k;
        float x01_len = x01.norm();
        float x01_len2 = x01_len * x01_len;
        Matrix<float, 3, 3> xxt = x01 * x01.transpose() / x01_len2;
        Matrix<float, 3, 3> He =
            stiffness_ * xxt + stiffness_ * (1.f - rest_length_ / x01_len) * (Matrix<float, 3, 3>::Identity() - xxt);
        Matrix<float, 6, 6> h;
        h.block<3, 3>(0, 0) = He;
        h.block<3, 3>(0, 3) = -He;
        h.block<3, 3>(3, 0) = -He;
        h.block<3, 3>(3, 3) = He;

        Vector3f x01_dir = x01.normalized();
        Vector3f fe = -stiffness_ * (x01_len - rest_length_) * x01_dir;
        Matrix<float, 6, 1> f;
        f.block<3, 1>(0, 0) = fe;
        f.block<3, 1>(3, 0) = -fe;

        Matrix<float, 6, 1> F = 1.f / dt2 * M * (xk - x - dt * v) - f;
        Matrix<float, 6, 6> H = 1.f / dt2 * M + h;
        Matrix<float, 6, 1> dx = -H.inverse() * F;
        xk += dx;
    }
}

void Spring::UpdateFe(Vector3f x0k, Vector3f x1k)
{
    Vector3f x01 = x0k - x1k;
    float x01_len = x01.norm();
    Vector3f x01_dir = x01.normalized();
    fe = -stiffness_ * (x01_len - rest_length_) * x01_dir;
}

void Spring::UpdateHe(Vector3f x0k, Vector3f x1k)
{
    Vector3f x01 = x0k - x1k;
    float x01_len = x01.norm();
    float x01_len2 = x01_len * x01_len;
    Matrix<float, 3, 3> xxt = x01 * x01.transpose() / x01_len2;
    if (rest_length_ > x01_len)
    {
        He = stiffness_ * xxt;
    }
    else
    {
        He = stiffness_ * xxt + stiffness_ * (1.f - rest_length_ / x01_len) * (Matrix<float, 3, 3>::Identity() - xxt);
    }
}