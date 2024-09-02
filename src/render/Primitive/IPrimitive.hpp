#pragma once

class IPrimitive
{
  public:
    virtual ~IPrimitive() = default;
    virtual void Draw() = 0;
};