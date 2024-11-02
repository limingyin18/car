#pragma once

#include <string>
#include <spdlog/spdlog.h>

class SDL_Window;
class Render;
class Game;

class App
{
public:
    App()
    {
        spdlog::debug("App construct");
    };
    ~App()
    {
        spdlog::debug("App destruct");
    };

    void Init();
    void Run();
    void Destroy();

private:
    void InitWSI();
    void InitIMGUI();
    void DrawUI();

    std::string title_ = "Borrow The East Wind";
    uint32_t width_ = 1280;
    uint32_t height_ = 960;
    std::shared_ptr<SDL_Window> window_ = nullptr;
};