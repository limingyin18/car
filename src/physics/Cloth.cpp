#include "Cloth.hpp"

#include "Eigen/Core"
#include "Particle.hpp"
#include "Spring.hpp"
#include "tools/Timer.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;
using namespace Eigen;
using namespace physics;

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
    static int count = 0;
    auto start = chrono::high_resolution_clock::now();
    float dt = Timer::GetInstance().GetDeltaT();
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

    for (int k = 0; k < 10; k++)
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
            // if(spring->i == 1 || spring->j == 1)
            // {
            //     spdlog::debug("i: {}, j: {}", spring->i, spring->j);
            //     stringstream ss;
            //     ss << spring_[s]->fe << std::endl;
            //     spdlog::info("spring_[s]->fe: \n {}", ss.str());
            // }
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

        // stringstream ss;
        // ss << xk_ << std::endl;
        // spdlog::info("xk_: \n {}", ss.str());
        // ss.clear();
        // ss.str("");

        // ss << f_ << std::endl;
        // spdlog::info("f_: \n {}", ss.str());

        bool is_nan = f_.array().isNaN().any();
        assert(!is_nan && "f_ is nan");

        f_.setZero();
        h_.setZero();

        float dx_len = dx_.norm();
        // std::cout << "dx_len: " << dx_len << std::endl;
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
        particles_[i]->pos_ = xk_.block<3, 1>(3 * i, 0);
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    // std::cout << "Cloth::Update: " << duration.count() << " microseconds" << std::endl;
    count++;
    // std::cout << count << std::endl;
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