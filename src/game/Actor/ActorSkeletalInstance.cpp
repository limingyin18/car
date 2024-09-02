#include "ActorSkeletalInstance.hpp"
#include "ActorInstance.hpp"

void ActorSkeletalInstance::CreatePrimitives()
{
    LoadAnimations();

    ActorInstance::CreatePrimitives();
}