#pragma once

#include "ActorSkeletalIndirect.hpp"

class ActorSkeletalIndirectLod : public ActorSkeletalIndirect
{
  public:
    void Init() override;

  protected:
    void CreatePrimitives() override;
};