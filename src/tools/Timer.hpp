#pragma once

#include "Singleton.hpp"
#include "System.hpp"
#include <cstdint>

class Timer : public Singleton<Timer>, public System
{
    friend class Singleton<Timer>;

  private:
    float dt_ = 0.f;
    float dt_physics_ = 1.f / 60.f;
    float all_t_ = 0.f;

    uint32_t frame_graphics_ = 0;
    uint32_t frame_physics_ = 0;

  public:
    void Update() override;

    float GetPhysicsFixedDeltaTime()
    {
        return dt_physics_;
    }

    void IncrementFrameGraphics()
    {
        frame_graphics_++;
    }

    uint32_t GetFrameGraphics()
    {
        return frame_graphics_;
    }

    void IncrementFramePhysics()
    {
        frame_physics_++;
    }

    uint32_t GetFramePhysics()
    {
        return frame_physics_;
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