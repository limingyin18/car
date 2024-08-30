#pragma once

#include "Actor.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

class ModelSkeletal;
class Animation;
class Animator;
class Shader;
class Primitive;

class ActorSkeletal : public virtual Actor
{
  public:
    ActorSkeletal() = default;
    ~ActorSkeletal() = default;

    void Init() override;
    void Update() override;

    [[nodiscard]] uint32_t GetAnimationCount() const
    {
        return animations_.size();
    }
    void SwitchAnimation(uint32_t index);

  protected:
    std::vector<std::shared_ptr<Animation>> animations_;
    std::shared_ptr<Animator> animator_ = nullptr;
};