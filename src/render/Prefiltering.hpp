#pragma once

#include <cstdint>
#include <memory>

class Shader;
class IMesh;

class Prefiltering final
{
  public:
    void Init(const std::shared_ptr<Shader> &shader, uint32_t envCubemap, const std::shared_ptr<IMesh> &cubeMesh,
              uint32_t width = 128, uint32_t height = 128);
    void Draw();
    [[nodiscard]] uint32_t GetPrefilterMap() const
    {
        return prefilterMap_;
    }

  private:
    uint32_t width_ = -1, height_ = -1;
    uint32_t fbo_ = -1, dpeth_texture_ = -1;
    uint32_t prefilterMap_ = -1;
    uint32_t envCubemap_ = -1;

    std::shared_ptr<Shader> shader_ = nullptr;
    std::shared_ptr<IMesh> cubeMesh_ = nullptr;
};