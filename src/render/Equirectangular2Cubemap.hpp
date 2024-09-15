#pragma once

#include "render/Mesh/IMesh.hpp"
#include <cstdint>
#include <memory>

class Shader;

class Equirectangular2Cubemap final
{
  public:
    void Init(const std::shared_ptr<Shader> &equirectangularToCubemapShader, uint32_t hdrTexture, uint32_t width,
              uint32_t height, const std::shared_ptr<IMesh> &cubeMesh);

    [[nodiscard]] uint32_t GetEnvCubemap() const
    {
        return envCubemap_;
    }
    void Draw();

  private:
    uint32_t fbo_ = -1, dpeth_texture_ = -1;
    uint32_t envCubemap_ = -1;
    uint32_t width_ = -1, height_ = -1;

    std::shared_ptr<Shader> equirectangularToCubemapShader_ = nullptr;
    uint32_t hdrTexture_ = -1;

    std::shared_ptr<IMesh> cubeMesh_ = nullptr;
};