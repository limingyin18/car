#include "Cloth.hpp"

#include "Eigen/Core"
#include "Particle.hpp"
#include "Spring.hpp"
#include "tools/Timer.hpp"

#include <cstdint>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace Eigen;
using namespace physics;

constexpr uint32_t kMaxIter = 32;

void Cloth::Init()
{
    m_ = particles_.size();
    x_ = MatrixXf::Zero(3 * m_, 1);
    dx_ = MatrixXf::Zero(3 * m_, 1);
    xk_ = MatrixXf::Zero(3 * m_, 1);
    v_ = MatrixXf::Zero(3 * m_, 1);
    f_ = MatrixXf::Zero(3 * m_, 1);
    h_ = MatrixXf::Zero(3 * m_, 3 * m_);

    MatrixXf mdiag = MatrixXf::Zero(3 * m_, 1);
    for (int i = 0; i < m_; ++i)
    {
        if (particles_[i]->mass_inv_ == 0.f)
        {
            mdiag.block<3, 1>(i * 3, 0) = Vector3f{}.setOnes() * 1000.f;
            continue;
        }
        mdiag.block<3, 1>(i * 3, 0) = Vector3f{}.setOnes() * 1.f / particles_[i]->mass_inv_;
    }
    M_ = mdiag.asDiagonal();
}

void Cloth::Update()
{
    UpdateImplicit();
    // UpdateImplicitOpimize();
}

void Cloth::UpdateImplicitOpimize()
{
    float dt = Timer::GetInstance().GetPhysicsFixedDeltaTime();
    float dt2 = dt * dt;

    for (int i = 0; i < m_; ++i)
    {
        x_.block<3, 1>(3 * i, 0) = particles_[i]->pos_;
    }
    for (int i = 0; i < m_; ++i)
    {
        xk_.block<3, 1>(3 * i, 0) = particles_[i]->pos_;
    }

    for (int i = 0; i < m_; ++i)
    {
        v_.block<3, 1>(3 * i, 0) = particles_[i]->velocity_;
    }

    for (int k = 0; k < 32; k++)
    {
        for (int i = 0; i < m_; ++i)
        {
            if (particles_[i]->mass_inv_ == 0.f)
            {
                continue;
            }
            f_(3 * i + 1, 0) = -1.f / particles_[i]->mass_inv_ * 10.f;
        }
        for (int s = 0; s < spring_.size(); ++s)
        {
            auto spring = spring_[s];
            Vector3f x0k = xk_.block<3, 1>(3 * spring->i, 0);
            Vector3f x1k = xk_.block<3, 1>(3 * spring->j, 0);
            spring->UpdateFe(x0k, x1k);

            f_.block<3, 1>(3 * spring->i, 0) += spring_[s]->fe;
            f_.block<3, 1>(3 * spring->j, 0) += -spring_[s]->fe;
        }

        F_ = 1.f / dt2 * M_ * (xk_ - x_ - dt * v_) - f_;
        for (int i = 0; i < m_; ++i)
        {
            float P = 1.f / particles_[i]->mass_inv_ / dt2 + 4.f;
            dx_.block<3, 1>(3 * i, 0) = -1.f / P * F_.block<3, 1>(3 * i, 0);
        }
        xk_ += dx_;

        f_.setZero();
        h_.setZero();

        float dx_len = dx_.norm();
        if (dx_len < 1e-2)
        {
            break;
        }
    }

    for (int i = 0; i < m_; ++i)
    {
        if (particles_[i]->mass_inv_ == 0.f)
        {
            continue;
        }
        particles_[i]->velocity_ = (xk_.block<3, 1>(3 * i, 0) - particles_[i]->pos_) / dt;
        // particles_[i]->pos_ = xk_.block<3, 1>(3 * i, 0);
    }
}

void Cloth::UpdateImplicit()
{
    float dt = Timer::GetInstance().GetPhysicsFixedDeltaTime();
    float dt2 = dt * dt;

    for (int i = 0; i < m_; ++i)
    {
        x_.block<3, 1>(3 * i, 0) = particles_[i]->pos_;
    }
    for (int i = 0; i < m_; ++i)
    {
        xk_.block<3, 1>(3 * i, 0) = particles_[i]->pos_;
    }

    for (int i = 0; i < m_; ++i)
    {
        v_.block<3, 1>(3 * i, 0) = particles_[i]->velocity_;
    }

    for (int k = 0; k < kMaxIter; k++)
    {
        for (int i = 0; i < m_; ++i)
        {
            if (particles_[i]->mass_inv_ == 0.f)
            {
                continue;
            }
            f_(3 * i + 1, 0) = -1.f / particles_[i]->mass_inv_ * 10.f;
        }
        for (int s = 0; s < spring_.size(); ++s)
        {
            auto spring = spring_[s];
            Vector3f x0k = xk_.block<3, 1>(3 * spring->i, 0);
            Vector3f x1k = xk_.block<3, 1>(3 * spring->j, 0);
            spring->UpdateFe(x0k, x1k);

            f_.block<3, 1>(3 * spring->i, 0) += spring_[s]->fe;
            f_.block<3, 1>(3 * spring->j, 0) += -spring_[s]->fe;
        }

        for (int s = 0; s < spring_.size(); ++s)
        {
            auto spring = spring_[s];
            Vector3f x0k = xk_.block<3, 1>(3 * spring->i, 0);
            Vector3f x1k = xk_.block<3, 1>(3 * spring->j, 0);
            spring->UpdateHe(x0k, x1k);

            h_.block<3, 3>(3 * spring->i, 3 * spring->i) += spring_[s]->He;
            h_.block<3, 3>(3 * spring->j, 3 * spring->j) += spring_[s]->He;

            h_.block<3, 3>(3 * spring->i, 3 * spring->j) += -spring_[s]->He;
            h_.block<3, 3>(3 * spring->j, 3 * spring->i) += -spring_[s]->He;
        }

        F_ = 1.f / dt2 * M_ * (xk_ - x_ - dt * v_) - f_;
        H_ = 1.f / dt2 * M_ + h_;
        auto H_inv = H_.inverse();
        dx_ = -H_inv * F_;

        xk_ += dx_;

        f_.setZero();
        h_.setZero();

        float dx_len = dx_.norm();
        if (dx_len < 1e-2)
        {
            break;
        }
    }

    for (int i = 0; i < m_; ++i)
    {
        if (particles_[i]->mass_inv_ == 0.f)
        {
            continue;
        }
        particles_[i]->velocity_ = (xk_.block<3, 1>(3 * i, 0) - particles_[i]->pos_) / dt;
        // particles_[i]->pos_ = xk_.block<3, 1>(3 * i, 0);
    }
}

void Cloth::AddParticle(const std::shared_ptr<Particle> &particle)
{
    particles_.push_back(particle);
}

void Cloth::AddSpring(int i, int j, const std::shared_ptr<Particle> &particle0,
                      const std::shared_ptr<Particle> &particle1, float stiffness)
{
    auto spring = make_shared<Spring>(i, j, particle0, particle1, stiffness);
    spring_.push_back(spring);
}