#pragma once

#include "Scene.hpp"

class Cloth;

class SceneClothSphere : public Scene
{
  public:
    void Init() override;
    void Update() override;

  private:
    void InitCloth();
    void InitSphere();

    void UpdateSphere();
    void CollideClothSphere();

    std::shared_ptr<Cloth> cloth_;
    std::shared_ptr<Actor> sphere_;
};