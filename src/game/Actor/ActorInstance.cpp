#include "ActorInstance.hpp"

#include "render/Primitive/PrimitiveInstance.hpp"

using namespace std;

void ActorInstance::CreatePrimitives()
{
    for (auto &mesh : meshes_)
    {
        auto primitive = make_shared<PrimitiveInstance>();
        primitive->Init(mesh, shader_, material_, instance_transforms_);
        primitives_.push_back(primitive);
    }
}

void ActorInstance::Update()
{
    for (auto &primitive : primitives_)
    {
        auto primitive_instance = dynamic_pointer_cast<PrimitiveInstance>(primitive);
        primitive_instance->SetInstanceTransforms(instance_transforms_);
    }
}