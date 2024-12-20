#pragma once

#include <memory>
#include <vector>

class IActor;
class Actor;
class ActorSkeletal;
class IPrimitive;
class Skybox;
union SDL_Event;

class Scene
{
  public:
    virtual void Init();
    virtual void Update();
    virtual void UpdatePhysics();

    virtual void ProcessInput(SDL_Event *event) {};

    [[nodiscard]] std::vector<std::shared_ptr<IActor>> &GetActors()
    {
        return actors_;
    }

    [[nodiscard]] std::vector<std::shared_ptr<IPrimitive>> GetPrimitives() const;

  protected:
    std::vector<std::shared_ptr<IActor>> actors_;

  private:
    void InitSkybox();
    void InitPlane();

    std::shared_ptr<Skybox> skybox_;
    std::shared_ptr<Actor> plane_;
};