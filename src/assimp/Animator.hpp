#pragma once

#include "Animation.hpp"

constexpr uint32_t MAX_BONES = 100u;

class Animator
{
  public:
    Animator(Animation *currentAnimation);

    void UpdateAnimation(float dt);

    void PlayAnimation(Animation *pAnimation);

    void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform);

    std::vector<glm::mat4> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }

  private:
    std::vector<glm::mat4> m_FinalBoneMatrices{};
    Animation *m_CurrentAnimation = nullptr;
    float m_CurrentTime = 0.0f;
    float m_DeltaTime = 0.0f;
};