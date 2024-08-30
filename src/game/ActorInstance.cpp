#include "ActorInstance.hpp"

#include "render/Primitive/PrimitiveInstance.hpp"

using namespace std;

void ActorInstance::CreatePrimitives()
{
    for (auto &mesh : meshes_)
    {
        auto primitive = make_shared<PrimitiveInstance>();
        primitive->Init(mesh, shader_, instance_transforms_);
        primitives_.push_back(primitive);
    }
}