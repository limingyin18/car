#pragma once

class IActor
{
  public:
    IActor() = default;
    virtual ~IActor() = default;

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Destroy() = 0;
};