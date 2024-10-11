#pragma once

#include "Scene.hpp"

#include "BasicActor/Cloth.hpp"

class SceneOneSpring : public Scene
{
  public:
    void Init() override;
    void Update() override;

  private:
    void InitPoints();
    void InitSpring();

    std::shared_ptr<IActor> points_;

    std::vector<Edge> edges_;
};