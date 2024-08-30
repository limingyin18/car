#include "ActorSkeletal.hpp"

#include "assimp/Animation.hpp"
#include "assimp/Animator.hpp"
#include "assimp/ModelSkeletal.hpp"

#include "render/Primitive/Primitive.hpp"
#include "render/Shader/Shader.hpp"
#include <memory>

using namespace std;

void ActorSkeletal::Init()
{
    // spdlog::debug("Actor Skeletal Init");

    model_ = make_shared<ModelSkeletal>();
    Actor::Init();

    auto scene = model_->GetScene();
    animations_.resize(scene->mNumAnimations);
    for (uint32_t i = 0; i < scene->mNumAnimations; i++)
    {
        animations_[i] = make_shared<Animation>(scene->mAnimations[i], scene, (ModelSkeletal *)model_.get());
    }
    animator_ = make_shared<Animator>(animations_[0].get());
}

void ActorSkeletal::Update()
{
    animator_->UpdateAnimation(0.01f);

    auto transforms = animator_->GetFinalBoneMatrices();
    shader_->use();
    for (size_t i = 0; i < transforms.size(); i++)
    {
        shader_->setMat4("finalBonesMatrices[" + to_string(i) + "]", transforms[i]);
    }
}

void ActorSkeletal::SwitchAnimation(uint32_t index)
{
    if (index >= animations_.size())
    {
        spdlog::error("Invalid animation index");
        return;
    }
    animator_->PlayAnimation(animations_[index].get());
}