#include "Game.hpp"

#include "Actor/Actor.hpp"
#include "render/Render.hpp"


using namespace std;

void Game::TestBasicGeometry()
{
    spdlog::info("TestBasicGeometry");

    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();

    auto cube = std::make_shared<Actor>();
    cube->AddMesh(meshes_map["cube"]);
    cube->SetShader(shaders_map["phong"]);
    actors_.push_back(cube);

    auto sphere = std::make_shared<Actor>();
    cube->AddMesh(meshes_map["sphere"]);
    cube->SetShader(shaders_map["phong"]);
    actors_.push_back(sphere);
}