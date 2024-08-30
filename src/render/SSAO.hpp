#pragma once

#include <vector>
#include <random>
#include <glm/glm.hpp>

#include "Camera.hpp"
#include "Shader/Shader.hpp"

/**
 * @brief SSAO
 *
 */
class SSAO
{
public:
    SSAO() = default;
    ~SSAO() = default;

    void Init(std::shared_ptr<Shader> _shader, std::shared_ptr<Shader> _blur_shader, Camera &_camera);
    void Use();
    void UseBlur();
    [[nodiscard]] uint32_t GetColorTexture() const { return color_texture_; }
    [[nodiscard]] uint32_t GetBlurColorTexture() const { return blur_color_texture_; }
    [[nodiscard]] uint32_t GetNoiseTexture() const { return noise_texture_; }
    [[nodiscard]] std::vector<glm::vec3> GetKernel() { return kernel_; }

private:
    void GenerateSampleKernel();
    void GenerateNoiseTexture();
    void CreateFramebuffer(uint32_t _width, uint32_t _height);
    void CreateBlurFramebuffer(uint32_t _width, uint32_t _height);

    std::uniform_real_distribution<float> random_floats_{0.f, 1.f}; // generates random floats between 0.0 and 1.0
    std::default_random_engine generator_;
    std::vector<glm::vec3> kernel_;
    std::vector<glm::vec3> noise_;
    uint32_t noise_texture_ = -1;

    uint32_t framebuffer_ = -1;
    uint32_t color_texture_ = -1;

    std::shared_ptr<Shader> shader_;

    std::shared_ptr<Shader> blur_shader_;
    uint32_t blur_framebuffer_ = -1;
    uint32_t blur_color_texture_ = -1;
};