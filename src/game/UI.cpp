#include "Game.hpp"

#include "tools/Timer.hpp"

#include <imgui.h>

void Game::DrawUI()
{
    auto &game = Game::GetInstance();
    auto &timer = Timer::GetInstance();

    ImGui::Begin("Cloth"); // Create a window called "Hello, world!" and append into it.

    std::string time_str = std::string("time: ") + std::to_string(timer.GetAllT());
    ImGui::Text("%s", time_str.c_str());

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / (ImGui::GetIO().Framerate),
                (ImGui::GetIO().Framerate));

    std::string str_frame_graphics = std::string("frame graphics: ") + std::to_string(timer.GetFrameGraphics());
    ImGui::Text("%s", str_frame_graphics.c_str());

    std::string str_frame_physics = std::string("frame physics: ") + std::to_string(timer.GetFramePhysics());
    ImGui::Text("%s", str_frame_physics.c_str());

    if (ImGui::Button("Scene"))
    {
        game.SwitchScene("scene");
    }

    if (ImGui::Button("Scene Cloth Cube"))
    {
        game.SwitchScene("scene_cloth_cube");
    }

    if (ImGui::Button("Scene Character"))
    {
        spdlog::info("scene character");
        game.SwitchScene("scene_character");
    }

    if (physics_simulation_)
    {
        if (ImGui::Button("physics pause"))
        {
            physics_simulation_ = false;
        }
    }
    else
    {
        if (ImGui::Button("physics start"))
        {
            physics_simulation_ = true;
        }
    }

    ImGui::End();
}