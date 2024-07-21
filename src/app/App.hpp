#pragma once

#include <string>
#include <spdlog/spdlog.h>

class SDL_Window;
class Render;

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

    std::string title_ = "Car Display";
    uint32_t width_ = 1080;
    uint32_t height_ = 960;
    std::shared_ptr<SDL_Window> window_ = nullptr;
    std::shared_ptr<Render> render_ = nullptr;
};