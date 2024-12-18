#pragma once

#include "Scene.hpp"

class SceneCharacter : public Scene
{
  public:
    void Init() override;
    void ProcessInput(SDL_Event *event) override;

  private:
    void InitHuman();
    std::shared_ptr<ActorSkeletal> human_;
};