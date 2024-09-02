#pragma once

#include "PrimitiveSkeletalIndirect.hpp"

class PrimitiveSkeletalIndirectLod : public PrimitiveSkeletalIndirect
{
  public:
    void Draw() override;

  protected:
    void CreateIndirectCommandBuffer() override;
    void CreateInstanceIDBuffer() override;
    std::vector<DrawElementsIndirectCommand> indirect_commands_;
};