#pragma once

#include "ActorInstance.hpp"
#include "ActorSkeletal.hpp"

class ActorSkeletalInstance : public ActorSkeletal, public ActorInstance
{
  protected:
    void CreatePrimitives() override;
};