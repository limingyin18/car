#pragma once

#include <memory>
#include <vector>

class IPrimitive;

class IActor
{
  public:
    IActor() = default;
    virtual ~IActor() = default;

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void UpdatePhysics() = 0;
    virtual void Destroy() = 0;

    virtual std::vector<std::shared_ptr<IPrimitive>> &GetPrimitives() = 0;
};