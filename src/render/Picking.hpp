#pragma once

#include <cstdint>
#include <memory>

class Shader;

class Picking
{
  public:
    Picking() = default;
    ~Picking() = default;

    void Init(const std::shared_ptr<Shader> &shader);
    void Use(int x_offset, int y_offset);
    std::shared_ptr<Shader> GetShader() const
    {
        return shader_;
    }

    uint32_t Pick();
    float depth_;

  private:
    uint32_t fbo_ = -1;
    uint32_t texture_index_ = -1;
    uint32_t texture_depth_ = -1;
    uint32_t width_ = -1, height_ = -1;
    uint32_t x_offset_ = -1, y_offset_ = -1;

    std::shared_ptr<Shader> shader_ = nullptr;
};