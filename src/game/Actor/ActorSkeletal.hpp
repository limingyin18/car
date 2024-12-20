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
    virtual void SwitchAnimation(uint32_t index);
    void AddAnimation(const std::string &path);

    void SetAnimated(bool is_animated)
    {
        is_animated_ = is_animated;
    }

  protected:
    std::vector<std::string> animation_paths_;
    void CreatePrimitives() override;
    void LoadAnimations();
    std::vector<std::shared_ptr<Animation>> animations_;

    std::shared_ptr<Animator> animator_ = nullptr;
    bool is_animated_ = false;
};