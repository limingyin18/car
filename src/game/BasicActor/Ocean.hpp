#pragma once

#include <complex>
#include <memory>
#include <random>

#include "game/Actor/Actor.hpp"

class Shader;

class OceanAmpl
{
  public:
    OceanAmpl(int n, int m, float lx, float lz, float a, float v, std::complex<float> w, float l);
    virtual ~OceanAmpl() = default;

    std::vector<std::complex<float>> h_tlide_0;
    std::vector<std::complex<float>> h_tlide_0_conj;
    float lambda; // choppy factor
  private:
    [[nodiscard]] int pos2idx(int n, int m) const;
    [[nodiscard]] std::complex<float> getK(int n, int m) const;

    [[nodiscard]] float dispersion(float k) const;
    [[nodiscard]] float Ph(std::complex<float> k) const;
    std::complex<float> hTlide0(std::complex<float> k);
    [[nodiscard]] std::complex<float> hTlide(int n, int m, float t) const;

    int N, M;                     // sampling points
    float Lx, Lz;                 // patch size
    float A;                      // amplitude
    float wind_speed;             // wind seed
    std::complex<float> wind_dir; // wind direction

    std::default_random_engine rand_generator;
    std::normal_distribution<float> normal_dist;
};

class Ocean : public Actor
{
  public:
    std::unique_ptr<OceanAmpl> ampl_ = nullptr;

  public:
    explicit Ocean() = default;
    virtual ~Ocean() = default;
    Ocean(const Ocean &rhs) = delete;
    Ocean(Ocean &&rhs) = delete;
    Ocean &operator=(const Ocean &rhs) = delete;
    Ocean &operator=(Ocean &&rhs) = delete;

    void Init() override;
    void Update() override;
    void SetShaderAmpl(const std::shared_ptr<Shader> &shader_ampl)
    {
        shader_ampl_ = shader_ampl;
    }
    void SetShaderIfft2(const std::shared_ptr<Shader> &shader_ifft2)
    {
        shader_ifft2_ = shader_ifft2;
    }
    void SetShaderSign(const std::shared_ptr<Shader> &shader_sign)
    {
        shader_sign_ = shader_sign;
    }

  protected:
    void CreatePrimitives() override;

  private:
    std::shared_ptr<Shader> shader_ampl_;
    std::shared_ptr<Shader> shader_ifft2_;
    std::shared_ptr<Shader> shader_sign_;
};