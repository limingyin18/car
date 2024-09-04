#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <spdlog/spdlog.h>

class Actor;
class ActorInstance;
class ActorIndirect;
class ActorSkeletal;
class ActorSkeletalInstance;
class ActorSkeletalIndirect;
class ActorSkeletalIndirectLod;
class Primitive;
class IMesh;
class Render;
class Shield;

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
    std::shared_ptr<ActorSkeletalIndirectLod> fox_;
    std::shared_ptr<Shield> shield_;
    std::shared_ptr<Actor> house_;
    std::shared_ptr<ActorIndirect> cube_;
    std::shared_ptr<Render> render_;
};