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
class Skybox;

class Game
{
  public:
    static Game &GetInstance()
    {
        static Game instance;
        return instance;
    }

    void Init();
    void Update();

    [[nodiscard]] std::vector<std::shared_ptr<Primitive>> GetPrimitives() const;

    void SwitchAnimation();

    ~Game()
    {
        spdlog::debug("Game destruct");
    };

    Game(const Game &) = delete;

  private:
    Game()
    {
        spdlog::debug("Game construct");
    };

    void TestBasicGeometry();

    void InitHouse();
    void InitShields();
    void InitArrows();
    void InitCubes();

    std::vector<std::shared_ptr<Actor>> actors_;

    uint32_t index = 0;
    std::shared_ptr<ActorSkeletalIndirectLod> shield_;
    std::shared_ptr<ActorSkeletalIndirectLod> arrow_;
    std::shared_ptr<Actor> house_;
    std::shared_ptr<ActorIndirect> cube_;
    std::shared_ptr<Skybox> skybox_;
};