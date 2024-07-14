#include "App.hpp"
#include "render/Render.hpp"

#include "glad/glad.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>

void App::Init()
{
    InitWSI();

    render_ = std::make_shared<Render>();
    render_->Init(width_, height_);
}

void App::InitWSI()
{
    spdlog::info("init wsi");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags window_flags{SDL_WINDOW_OPENGL};
    window_ = {SDL_CreateWindow(title_.c_str(), // window title
                                100, 100,
                                width_,  // window width in pixels
                                height_, // window height in pixels
                                window_flags),
               SDL_DestroyWindow};
    if (!window_)
    {
        throw std::runtime_error("window crete failed.");
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window_.get());
    if (!gl_context)
    {
        throw std::runtime_error("opengl context crete failed.");
    }
    SDL_GL_MakeCurrent(window_.get(), gl_context);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    if (SDL_GL_SetSwapInterval(1) < 0)
    {
        spdlog::warn("set swap interval failed");
    }
}

void App::Run()
{
    spdlog::info("app run");

    SDL_Event event;
    do
    {
        // auto start = std::chrono::high_resolution_clock::now();
        while (SDL_PollEvent(&event) != 0 && event.type != SDL_QUIT)
        {
            // LOGI(event.type);
            // engine_->processInputEvent(event, engine_->input);
            // ImGui_ImplSDL3_ProcessEvent(&event);
        }
        render_->Update();
        // update();
        // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        // LOGI("duration: {}", duration.count());
        // if (engine_->input.keys["esc"])
        //    break;

        // PRESENT BACKBUFFER:
        SDL_GL_SwapWindow(window_.get());
    } while (event.type != SDL_QUIT);
}