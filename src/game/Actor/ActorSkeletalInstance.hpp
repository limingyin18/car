#pragma once

#include "ActorInstance.hpp"
#include "ActorSkeletal.hpp"

class ActorSkeletalInstance : public ActorSkeletal, public ActorInstance
{
  protected:
    void Update() override
    {
        ActorSkeletal::Update();
        ActorInstance::Update();
    }
    void CreatePrimitives() override;
};