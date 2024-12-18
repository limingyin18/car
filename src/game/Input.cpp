#include "Game.hpp"
#include "render/Render.hpp"

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

void Game::ProcessInput(SDL_Event *event)
{
    auto &render = Render::GetInstance();
    auto camera = render.GetCamera();
    float speed = 0.5f;
    switch (event->type)
    {
    case SDL_KEYDOWN:
        // spdlog::info("key down");
        switch (event->key.keysym.sym)
        {
        case SDLK_w:
            camera->MoveForward(1.0f * speed);
            break;
        case SDLK_s:
            camera->MoveForward(-1.0f * speed);
            break;
        case SDLK_a:
            camera->MoveRight(-1.0f * speed);
            break;
        case SDLK_d:
            camera->MoveRight(1.0f * speed);
            break;
        case SDLK_q:
            camera->MoveUp(1.0f * speed);
            break;
        case SDLK_e:
            camera->MoveUp(-1.0f * speed);
            break;
        case SDLK_r:
            camera->RotatePitch(1.0f * speed);
            break;
        case SDLK_c:
            camera->RotatePitch(-1.0f * speed);
            break;
        case SDLK_z:
            camera->RotateYaw(1.0f * speed);
            break;
        case SDLK_x:
            camera->RotateYaw(-1.0f * speed);
            break;
        case SDLK_f:
            scene_->ProcessInput(event);
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
        int px = event->motion.x;
        int py = event->motion.y;
        render.mouse_pos_x_last = render.mouse_pos_x;
        render.mouse_pos_y_last = render.mouse_pos_y;
        render.mouse_pos_x = px;
        render.mouse_pos_y = py;
        // spdlog::info("mouse motion: x{}, y{}", px, py);
        break;
    }
    case SDL_MOUSEBUTTONDOWN:
        // render.SetPicking(true);
        render.mouse_pos_x_last = render.mouse_pos_x;
        render.mouse_pos_y_last = render.mouse_pos_y;

        render.mouse_pos_x = render.mouse_pos_x;
        render.mouse_pos_y = render.mouse_pos_y;
        break;
    case SDL_MOUSEBUTTONUP:
        render.SetSelected(false);
        break;
    default:
        break;
    }
}