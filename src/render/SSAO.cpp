#include "SSAO.hpp"

namespace
{
    float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }
}

void SSAO::Use()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTextureUnit(2, noise_texture_);
    shader_->use();
}

void SSAO::UseBlur()
{
    glBindFramebuffer(GL_FRAMEBUFFER, blur_framebuffer_);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTextureUnit(0, color_texture_);
    blur_shader_->use();
}



void SSAO::Init(std::shared_ptr<Shader> _shader, std::shared_ptr<Shader> _blur_shader, Camera &_camera)
{
    shader_ = _shader;
    blur_shader_ = _blur_shader;

    GenerateSampleKernel();
    GenerateNoiseTexture();
    CreateFramebuffer(_camera.GetWidth(), _camera.GetHeight());
    CreateBlurFramebuffer(_camera.GetWidth(), _camera.GetHeight());

    shader_->use();
    for (unsigned int i = 0; i < 64; ++i)
        shader_->setVec3("samples[" + std::to_string(i) + "]", kernel_[i]);
    shader_->setMat4("projection", _camera.GetProj());
    shader_->setFloat("width", float(_camera.GetWidth()));
    shader_->setFloat("height", float(_camera.GetHeight()));
}

void SSAO::GenerateSampleKernel()
{
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(random_floats_(generator_) * 2.f - 1.f,
                         random_floats_(generator_) * 2.f - 1.f, random_floats_(generator_));
        sample = glm::normalize(sample);
        sample *= random_floats_(generator_);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        // lerp
        sample *= scale;
        kernel_.push_back(sample);
    }
}

void SSAO::GenerateNoiseTexture()
{
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(random_floats_(generator_) * 2.f - 1.f, random_floats_(generator_) * 2.f - 1.f, 0.f); // rotate around z-axis (in tangent space)
        noise_.push_back(noise);
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &noise_texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureStorage2D(noise_texture_, 1, GL_RGB32F, 4, 4);
    glTextureSubImage2D(noise_texture_, 0, 0, 0, 4, 4, GL_RGB, GL_FLOAT, noise_.data());
}

void SSAO::CreateFramebuffer(uint32_t _width, uint32_t _height)
{
    // create framebuffer
    glCreateFramebuffers(1, &framebuffer_);

    // create texture to hold SSAO
    glCreateTextures(GL_TEXTURE_2D, 1, &color_texture_);
    glTextureParameteri(color_texture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(color_texture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(color_texture_, 1, GL_R32F, _width, _height);
    glNamedFramebufferTexture(framebuffer_, GL_COLOR_ATTACHMENT0, color_texture_, 0);

    // check if framebuffer is complete
    GLenum status = glCheckNamedFramebufferStatus(framebuffer_, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        spdlog::error(status);
        throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
}

void SSAO::CreateBlurFramebuffer(uint32_t _width, uint32_t _height)
{
    glCreateFramebuffers(1, &blur_framebuffer_);

    glCreateTextures(GL_TEXTURE_2D, 1, &blur_color_texture_);
    glTextureParameteri(blur_color_texture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(blur_color_texture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(blur_color_texture_, 1, GL_R32F, _width, _height);
    glNamedFramebufferTexture(blur_framebuffer_, GL_COLOR_ATTACHMENT0, blur_color_texture_, 0);
}