#pragma once

#include "Primitive.hpp"
#include "glm/fwd.hpp"

class PrimitiveInstance : public Primitive
{
  public:
    void Draw() override;

    virtual void Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
                      const std::vector<glm::mat4> &instance_transforms);

    void SetInstanceTransforms(const std::vector<glm::mat4> &instance_transforms)
    {
        instance_transforms_ = instance_transforms;
    }

  protected:
    std::vector<glm::mat4> instance_transforms_;
    uint32_t instance_transforms_buffer_;
};