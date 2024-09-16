#pragma once

#include <cstdint>
#include <memory>

class Shader;
class IMesh;

class Brdf
{
  private:
    uint32_t width_ = -1, height_ = -1;
    uint32_t fbo_ = -1, dpeth_texture_ = -1;
    uint32_t brdflut_texture_ = -1;

    std::shared_ptr<Shader> shader_ = nullptr;
    std::shared_ptr<IMesh> quadMesh_ = nullptr;

  public:
    void Draw();
    void Init(uint32_t width = 512, uint32_t height = 512);

    [[nodiscard]] uint32_t GetBRDF() const
    {
        return brdflut_texture_;
    }
};