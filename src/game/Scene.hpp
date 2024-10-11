#pragma once

#include <memory>
#include <vector>

class IActor;
class IPrimitive;
class Skybox;

class Scene
{
  public:
    virtual void Init();
    virtual void Update();

    [[nodiscard]] std::vector<std::shared_ptr<IActor>> &GetActors()
    {
        return actors_;
    }

    [[nodiscard]] std::vector<std::shared_ptr<IPrimitive>> GetPrimitives() const;

  protected:
    std::vector<std::shared_ptr<IActor>> actors_;

  private:
    void InitSkybox();
    std::shared_ptr<Skybox> skybox_;
};