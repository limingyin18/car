#include "Actor.hpp"

#include "assimp/Model.hpp"
#include "render/Primitive/Primitive.hpp"

#include <spdlog/spdlog.h>

using namespace std;

Actor::Actor()
{
    // spdlog::debug("Actor construct");
}

Actor::~Actor()
{
    // spdlog::debug("Actor destruct");
}

void Actor::Init()
{
    // spdlog::debug("Actor Init");

    if (model_ == nullptr)
    {
        model_ = make_shared<Model>();
    }

    if (model_path_ != "")
    {
        model_->Load(model_path_);
        meshes_ = model_->GetMeshes();
    }

    CreatePrimitives();
}

void Actor::CreatePrimitives()
{
    for (auto &mesh : meshes_)
    {
        auto primitive = make_shared<Primitive>();
        primitive->Init(mesh, shader_, transform_);
        primitives_.push_back(primitive);
    }
}