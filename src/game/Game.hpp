#pragma once

#include "Scene.hpp"

#include <spdlog/spdlog.h>
#include <unordered_map>

class IActor;

class Game
{
  public:
    static Game &GetInstance()
    {
        static Game instance;
        return instance;
    }

    void Init();

    void Update()
    {
        scene_->Update();
    }

    void SwitchScene(std::string scene_name);

    [[nodiscard]] std::vector<std::shared_ptr<IActor>> &GetActors() const
    {
        return scene_->GetActors();
    }

    [[nodiscard]] std::vector<std::shared_ptr<IPrimitive>> GetPrimitives() const
    {
        return scene_->GetPrimitives();
    }

    ~Game()
    {
        spdlog::debug("Game destruct");
    };

    Game(const Game &) = delete;

  private:
    Game();

    void InitScene();
    void InitSceneOneSpring();
    void InitSceneCloth();
    void InitSceneClothSphere();
    std::unordered_map<std::string, std::function<void()>> scene_map_init_;

    void InitBasicGeometry();
    void InitHouse();
    void InitShields();
    void InitArrows();
    void InitCubes();
    void InitPlane();
    void InitOcean();
    void InitCloth();

    std::unique_ptr<Scene> scene_;
};