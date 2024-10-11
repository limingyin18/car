#pragma once

class System
{
  public:
    virtual ~System() = default;

    virtual void Init()
    {
    }

    virtual void Update()
    {
    }

    virtual void Destroy()
    {
    }
};