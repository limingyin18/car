#pragma once

#include "Actor.hpp"

class ActorInstance : public virtual Actor
{
  public:
    void SetInstanceTransforms(const std::vector<glm::mat4> &instance_transforms)
    {
        instance_transforms_ = instance_transforms;
    }

    [[nodiscard]] std::vector<glm::mat4> &GetInstanceTransforms()
    {
        return instance_transforms_;
    }

    void Update() override;

  protected:
    void CreatePrimitives() override;

    std::vector<glm::mat4> instance_transforms_;
};