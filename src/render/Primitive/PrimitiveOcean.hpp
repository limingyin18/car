#pragma once

#include "Primitive.hpp"

#include <complex>

class PrimitiveOcean : public Primitive
{
  public:
    PrimitiveOcean() = default;
    virtual ~PrimitiveOcean() = default;
    void Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
              const std::shared_ptr<Shader> &shader_ampl, const std::shared_ptr<Shader> &shader_ifft2,
              const std::shared_ptr<Shader> &shader_sign, uint32_t n, uint32_t m, float lx, float lz,
              const std::vector<std::complex<float>> &h_tlide_0,
              const std::vector<std::complex<float>> &h_tlide_0_conj);
    void Draw() override;

  private:
    void createBufferOcean();
    void createAmpl();
    void createIfft2();
    void createSign();

    void ComputePass();
    void RenderPass();

    uint32_t buffer_ampl_ = -1;
    uint32_t buffer_normal_x_ = -1, buffer_normal_z_ = -1;
    uint32_t buffer_dx_ = -1, buffer_dz_ = -1;
    uint32_t buffer_h_tlide_0_ = -1, buffer_h_tlide_0_conj_ = -1;

    std::shared_ptr<Shader> shader_ampl_ = nullptr;
    std::shared_ptr<Shader> shader_ifft2_ = nullptr;
    std::shared_ptr<Shader> shader_sign_ = nullptr;
};