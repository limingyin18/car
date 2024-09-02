#pragma once

#include <random>
#include <string>

class Tool
{
  public:
    static int LoadTexture(std::string path);

    static Tool &GetInstance()
    {
        static Tool instance;
        return instance;
    }

    [[nodiscard]] float RandomFloat()
    {
        return distrib_(gen_);
    }

  private:
    Tool() : gen_(rd_()), distrib_(0.0f, 1.0f)
    {
    }
    ~Tool() = default;

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<float> distrib_;
};