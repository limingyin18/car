#pragma once

#include "Scene.hpp"

class Particle;

class SceneTwoTriangle : public Scene
{
  public:
    void Init() override;
    void Update() override;

    void UpdatePhysics() override;

  private:
    void InitTwoTriangle();

    std::shared_ptr<Actor> triangle_a_;
    std::shared_ptr<Actor> triangle_b_;
    std::vector<std::shared_ptr<Particle>> particles_a_;
    std::vector<std::shared_ptr<Particle>> particles_b_;
};