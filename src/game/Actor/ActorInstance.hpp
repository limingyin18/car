#pragma once

#include "Actor.hpp"

class ActorInstance : public virtual Actor
{
  public:
    void SetInstanceTransforms(const std::vector<glm::mat4> &instance_transforms)
    {
        instance_transforms_ = instance_transforms;
    }

  protected:
    void CreatePrimitives() override;

    std::vector<glm::mat4> instance_transforms_;
};