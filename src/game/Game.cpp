#include "Game.hpp"

#include "Actor/Actor.hpp"
#include "SceneOneSpring.hpp"
#include "SceneCloth.hpp"
#include "game/Scene.hpp"
#include "physics/PhysicsSystem.hpp"
#include "tools/Timer.hpp"

#include <memory>
#include <spdlog/spdlog.h>

using namespace std;

Game::Game()
{
    spdlog::debug("Game construct");

    scene_map_init_["scene"] = [this]() { InitScene(); };
    scene_map_init_["scene_onespring"] = [this]() { InitSceneOneSpring(); };
    scene_map_init_["scene_cloth"] = [this]() { InitSceneCloth(); };
}

void Game::Init()
{
    InitScene();
}

void Game::SwitchScene(string scene_name)
{
    auto &physics_system = PhysicsSystem::GetInstance();
    physics_system.Clear();

    scene_map_init_[scene_name]();
}

void Game::InitScene()
{
    scene_ = make_unique<Scene>();
    scene_->Init();
}

void Game::InitSceneOneSpring()
{
    scene_ = make_unique<SceneOneSpring>();
    scene_->Init();
}

void Game::InitSceneCloth()
{
    scene_ = make_unique<SceneCloth>();
    scene_->Init();
}