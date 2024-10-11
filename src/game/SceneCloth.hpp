#pragma once

#include "BasicActor/Cloth.hpp"
#include "Scene.hpp"


class SceneCloth : public Scene
{
  public:
    void Init() override;
    void Update() override;

  private:
    void InitCloth();

    std::shared_ptr<Cloth> cloth_;
};