#include "Render.hpp"

#include <spdlog/spdlog.h>
#include <glad/glad.h>

using namespace std;

void Render::CreateMultisampledFramebuffer()
{
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &multisampled_color_buffer_);
    glTextureStorage2DMultisample(multisampled_color_buffer_, sample_count_, GL_RGBA16, width_, height_, GL_TRUE);

    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &multisampled_depth_buffer_);
    glTextureStorage2DMultisample(multisampled_depth_buffer_, sample_count_, GL_DEPTH24_STENCIL8, width_, height_, GL_TRUE);

    glCreateFramebuffers(1, &multisampled_framebuffer_);
    glNamedFramebufferTexture(multisampled_framebuffer_, GL_COLOR_ATTACHMENT0, multisampled_color_buffer_, 0);
    glNamedFramebufferTexture(multisampled_framebuffer_, GL_DEPTH_STENCIL_ATTACHMENT, multisampled_depth_buffer_, 0);
    GLenum status = glCheckNamedFramebufferStatus(multisampled_framebuffer_, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        spdlog::error(status);
        throw runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
}

void Render::CreateShadowFramebuffer()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &shadow_texture_);
    glTextureStorage2D(shadow_texture_, 1, GL_DEPTH_COMPONENT32, shadow_width_, shadow_height_);
    glTextureParameteri(shadow_texture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(shadow_texture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(shadow_texture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(shadow_texture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTextureParameterfv(shadow_texture_, GL_TEXTURE_BORDER_COLOR, borderColor);

    glCreateFramebuffers(1, &shadow_framebuffer_);
    glNamedFramebufferTexture(shadow_framebuffer_, GL_DEPTH_ATTACHMENT, shadow_texture_, 0);
    glNamedFramebufferDrawBuffer(shadow_framebuffer_, GL_NONE);
    glNamedFramebufferReadBuffer(shadow_framebuffer_, GL_NONE);
}

void Render::CreateTestFramebuffer()
{
    glCreateFramebuffers(1, &test_fbo_);
    // - position color buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &test_color_texture);
    glTextureStorage2D(test_color_texture, 1, GL_RGBA32F, width_, height_);
    glTextureParameteri(test_color_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(test_color_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(test_color_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(test_color_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glNamedFramebufferTexture(test_fbo_, GL_COLOR_ATTACHMENT0, test_color_texture, 0);

    std::array<uint32_t, 1> attachments{GL_COLOR_ATTACHMENT0};

    glNamedFramebufferDrawBuffers(test_fbo_, attachments.size(), attachments.data());

    // depth
    glCreateTextures(GL_TEXTURE_2D, 1, &test_dpeth_texture_);
    glTextureStorage2D(test_dpeth_texture_, 1, GL_DEPTH_COMPONENT32, width_, height_);
    glTextureParameteri(test_dpeth_texture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(test_dpeth_texture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTextureParameteri(test_dpeth_texture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(test_dpeth_texture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glNamedFramebufferTexture(test_fbo_, GL_DEPTH_ATTACHMENT, test_dpeth_texture_, 0);

    GLenum status = glCheckNamedFramebufferStatus(test_fbo_, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        spdlog::error(status);
        throw runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
}