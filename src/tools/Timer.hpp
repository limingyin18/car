#pragma once

#include "Singleton.hpp"
#include "System.hpp"

class Timer : public Singleton<Timer>, public System
{
    friend class Singleton<Timer>;

  private:
    float dt_ = 0.f;
    float all_t_ = 0.f;

  public:
    void Update() override;

    float GetPhysicsFixedDeltaTime()
    {
        return 1.f/60.f;
    }

    float GetDeltaT()
    {
        return dt_;
    }

    float GetAllT()
    {
        return all_t_;
    }
};