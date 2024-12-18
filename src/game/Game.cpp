#include "Game.hpp"

#include "Actor/Actor.hpp"
#include "SceneCharacter.hpp"
#include "SceneCloth.hpp"
#include "SceneClothCube.hpp"
#include "SceneClothSphere.hpp"
#include "SceneOneSpring.hpp"
#include "SceneTwoTriangle.hpp"
#include "game/Scene.hpp"
#include "physics/PhysicsSystem.hpp"
#include "render/Render.hpp"
#include "tools/Timer.hpp"

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include <memory>
#include <spdlog/spdlog.h>

using namespace std;

Game &Game::GetInstance()
{
    static Game instance;
    return instance;
}

Game::Game()
{
    spdlog::debug("game construct");

    scene_map_init_["scene"] = [this]() { scene_ = make_unique<Scene>(); };
    scene_map_init_["scene_cloth_cube"] = [this]() { scene_ = make_unique<SceneClothCube>(); };
    scene_map_init_["scene_character"] = [this]() { scene_ = make_unique<SceneCharacter>(); };
}

void Game::Init()
{
    InitWSI();
    InitIMGUI();

    auto &render = Render::GetInstance();
    render.SetWidth(width_);
    render.SetHeight(height_);
    render.Init();

    auto &physics_system = PhysicsSystem::GetInstance();
    physics_system.Init();

    auto &timer = Timer::GetInstance();
    timer.Init();

    SwitchScene("scene");
}

void Game::SwitchScene(string const &scene_name)
{
    auto &physics_system = PhysicsSystem::GetInstance();
    physics_system.Clear();

    scene_map_init_[scene_name]();
    scene_->Init();
}

void Game::InitWSI()
{
    spdlog::debug("init wsi");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags window_flags{SDL_WINDOW_OPENGL};
    window_ = {SDL_CreateWindow(title_.c_str(), // window title
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

void Game::InitIMGUI()
{
    spdlog::debug("init imgui");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window_.get(), SDL_GL_GetCurrentContext());
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Game::Run()
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
            ProcessInput(&event);
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        scene_->Update();

        if (physics_simulation_)
        {
            scene_->UpdatePhysics();
            spdlog::debug(timer.GetFramePhysics());
            timer.IncrementFramePhysics();
        }

        render.Draw(game.GetPrimitives());

        DrawUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // PRESENT BACKBUFFER:
        SDL_GL_SwapWindow(window_.get());
        timer.IncrementFrameGraphics();
    } while (event.type != SDL_QUIT);
}
