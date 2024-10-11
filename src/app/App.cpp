#include "App.hpp"
#include "SDL_keycode.h"
#include "game/Game.hpp"
#include "physics/PhysicsSystem.hpp"
#include "render/Render.hpp"
#include "tools/Timer.hpp"

#include "glad/glad.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_video.h>
#include <memory>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

void ProcessInput(SDL_Event event, Camera &camera, Game &game, Render &render);

void App::Destroy()
{
    spdlog::debug("App Destroy");
}

void App::Init()
{
    InitWSI();
    InitIMGUI();

    auto &render = Render::GetInstance();
    render.SetWidth(width_);
    render.SetHeight(height_);
    render.Init();

    auto &physics_system = PhysicsSystem::GetInstance();
    physics_system.Init();

    auto &game = Game::GetInstance();
    game.Init();

    auto &timer = Timer::GetInstance();
    timer.Init();
}

void App::InitWSI()
{
    spdlog::info("init wsi");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags window_flags{SDL_WINDOW_OPENGL};
    window_ = {SDL_CreateWindow(title_.c_str(), // window title
                                0, 0,
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

void App::InitIMGUI()
{
    spdlog::info("init imgui");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window_.get(), SDL_GL_GetCurrentContext());
    ImGui_ImplOpenGL3_Init("#version 460");
}

void App::Run()
{
    spdlog::info("app run");

    auto &timer = Timer::GetInstance();
    auto &game = Game::GetInstance();
    auto &render = Render::GetInstance();
    auto &physics_system = PhysicsSystem::GetInstance();
    SDL_Event event;
    do
    {
        timer.Update();

        while (SDL_PollEvent(&event) != 0 && event.type != SDL_QUIT)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            ProcessInput(event, *render.GetCamera(), game, render);
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        game.Update();

        physics_system.Update();

        render.Draw(game.GetPrimitives());

        DrawUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // PRESENT BACKBUFFER:
        SDL_GL_SwapWindow(window_.get());
    } while (event.type != SDL_QUIT);
}

void ProcessInput(SDL_Event event, Camera &camera, Game &game, Render &render)
{
    float speed = 0.5f;
    switch (event.type)
    {
    case SDL_KEYDOWN:
        // spdlog::info("key down");
        switch (event.key.keysym.sym)
        {
        case SDLK_w:
            camera.MoveForward(1.0f * speed);
            break;
        case SDLK_s:
            camera.MoveForward(-1.0f * speed);
            break;
        case SDLK_a:
            camera.MoveRight(-1.0f * speed);
            break;
        case SDLK_d:
            camera.MoveRight(1.0f * speed);
            break;
        case SDLK_q:
            camera.MoveUp(1.0f * speed);
            break;
        case SDLK_e:
            camera.MoveUp(-1.0f * speed);
            break;
        case SDLK_r:
            camera.RotatePitch(1.0f * speed);
            break;
        case SDLK_c:
            camera.RotatePitch(-1.0f * speed);
            break;
        case SDLK_z:
            camera.RotateYaw(1.0f * speed);
            break;
        case SDLK_x:
            camera.RotateYaw(-1.0f * speed);
            break;
        case SDLK_f:
            // game.SwitchAnimation();
            break;
        case SDLK_ESCAPE:
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event);
            break;
        default:
            break;
        }
        break;
    case SDL_KEYUP:
        // spdlog::info("key up");
        break;
    case SDL_MOUSEMOTION: {
        int px = event.motion.x;
        int py = event.motion.y;
        render.mouse_pos_x = px;
        render.mouse_pos_y = py;
        // spdlog::info("mouse motion: x{}, y{}", px, py);
        break;
    }
    case SDL_MOUSEBUTTONDOWN:
        // render.SetPicking(true);
        render.mouse_pos_x_last = render.mouse_pos_x;
        render.mouse_pos_y_last = render.mouse_pos_y;
        break;
    case SDL_MOUSEBUTTONUP:
        render.SetPicking(false);
        break;
    default:
        break;
    }
}

void App::DrawUI()
{
    ImGui::Begin("Cloth"); // Create a window called "Hello, world!" and append into it.

    if (ImGui::Button("Scene 1"))
    {
        spdlog::info("Scene 1");
        auto &game = Game::GetInstance();
        game.SwitchScene("scene");
    }

    if (ImGui::Button("Scene 2"))
    {
        spdlog::info("Scene 2");

        auto &game = Game::GetInstance();
        game.SwitchScene("scene_onespring");
    }

    if (ImGui::Button("Scene 3"))
    {
        spdlog::info("Scene 3");

        auto &game = Game::GetInstance();
        game.SwitchScene("scene_cloth");
    }

    ImGui::End();
}