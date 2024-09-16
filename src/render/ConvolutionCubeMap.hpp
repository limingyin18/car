#pragma once

#include <cstdint>
#include <memory>

class Shader;
class IMesh;

class ConvolutionCubeMap
{
  private:
    uint32_t width_ = -1, height_ = -1;
    uint32_t fbo_ = -1, dpeth_texture_ = -1;
    uint32_t irradienceMap_ = -1;
    uint32_t envCubemap_ = -1;

    std::shared_ptr<Shader> shader_ = nullptr;
    std::shared_ptr<IMesh> cubeMesh_ = nullptr;


  public:
    void Draw();
    void Init(const std::shared_ptr<Shader> &shader, uint32_t envCubemap, const std::shared_ptr<IMesh> &cubeMesh,
              uint32_t width = 32, uint32_t height = 32);

    [[nodiscard]] uint32_t GetIrradianceMap() const
    {
        return irradienceMap_;
    }
};