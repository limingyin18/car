#pragma once

#include "render/Primitive/Primitive.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <spdlog/spdlog.h>

class Actor;
class ActorInstance;
class ActorSkeletal;
class ActorSkeletalInstance;
class IMesh;
class Render;

class Game
{
  public:
    Game()
    {
        spdlog::debug("Game construct");
    };
    ~Game()
    {
        spdlog::debug("Game destruct");
    };

    void Init(const std::shared_ptr<Render> &render);
    void Update();

    [[nodiscard]] std::vector<std::shared_ptr<Primitive>> GetPrimitives() const;

    void SwitchFoxAnimation();

  private:
    std::vector<std::shared_ptr<Actor>> actors_;
    std::shared_ptr<ActorSkeletalInstance> fox_;
    std::shared_ptr<Actor> house_;
    std::shared_ptr<ActorInstance> cube_;
    std::shared_ptr<Render> render_;
};