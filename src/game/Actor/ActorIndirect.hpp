#pragma once

#include "ActorInstance.hpp"

class ActorIndirect : public virtual ActorInstance
{
  public:
    void SetShaderCulling(const std::shared_ptr<Shader> &shader_culling)
    {
        shader_culling_ = shader_culling;
    }

  protected:

    void CreatePrimitives() override;
    std::shared_ptr<Shader> shader_culling_;
};