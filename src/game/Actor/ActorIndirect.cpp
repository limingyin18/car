#include "ActorIndirect.hpp"

#include "render/Primitive/PrimitiveIndirect.hpp"

using namespace std;

void ActorIndirect::CreatePrimitives()
{
    for (auto &mesh : meshes_)
    {
        auto primitive = make_shared<PrimitiveIndirect>();
        primitive->Init(mesh, shader_, shader_culling_, instance_transforms_);
        primitives_.push_back(primitive);
    }
}
