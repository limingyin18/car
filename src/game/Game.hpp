#pragma once

#include "Scene.hpp"
#include <cstdint>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

class IActor;
class SDL_Window;
union SDL_Event;

class Game
{
  private:
    std::string title_ = "app";
    uint32_t width_ = 1280;
    uint32_t height_ = 960;
    std::shared_ptr<SDL_Window> window_ = nullptr;

    std::unordered_map<std::string, std::function<void()>> scene_map_init_;
    std::unique_ptr<Scene> scene_;

    void InitWSI();
    void InitIMGUI();
    void DrawUI();

    void ProcessInput(SDL_Event *event);

  public:
    static Game &GetInstance();

    void Init();

    void Run();

    void Destroy()
    {
        spdlog::debug("game destroy");
    }

    void SwitchScene(std::string const&scene_name);

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
        spdlog::debug("game destruct");
    };

    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    Game(Game &&) = delete;
    Game &operator=(Game &&) = delete;

  private:
    Game();

    bool physics_simulation_ = false;
};