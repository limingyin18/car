#pragma once

#include "game/Actor/Actor.hpp"

class Skybox final : public Actor
{
  public:
    Skybox() = default;
    ~Skybox() = default;

    void Init() override;
    void Update() override;
};