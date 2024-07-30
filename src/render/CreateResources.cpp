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

void Render::CreateGBuffer()
{
    glCreateFramebuffers(1, &gBufferFBO_);

    // - position color buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &gPosition_);
    glTextureStorage2D(gPosition_, 1, GL_RGBA16F, width_, height_);
    glTextureParameteri(gPosition_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(gPosition_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(gPosition_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(gPosition_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glNamedFramebufferTexture(gBufferFBO_, GL_COLOR_ATTACHMENT0, gPosition_, 0);

    // - normal color buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &gNormal_);
    glTextureStorage2D(gNormal_, 1, GL_RGBA16F, width_, height_);
    glTextureParameteri(gNormal_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(gNormal_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glNamedFramebufferTexture(gBufferFBO_, GL_COLOR_ATTACHMENT1, gNormal_, 0);

    // - color + specular color buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &gAlbedoSpec_);
    glTextureStorage2D(gAlbedoSpec_, 1, GL_RGBA16F, width_, height_);
    glTextureParameteri(gAlbedoSpec_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(gAlbedoSpec_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glNamedFramebufferTexture(gBufferFBO_, GL_COLOR_ATTACHMENT2, gAlbedoSpec_, 0);

    std::array<uint32_t, 3> attachments{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};

    glNamedFramebufferDrawBuffers(gBufferFBO_, attachments.size(), attachments.data());

    // depth
    glCreateTextures(GL_TEXTURE_2D, 1, &gDepth_);
    glTextureStorage2D(gDepth_, 1, GL_DEPTH_COMPONENT32, width_, height_);
    glTextureParameteri(gDepth_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(gDepth_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(gDepth_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(gDepth_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTextureParameterfv(gDepth_, GL_TEXTURE_BORDER_COLOR, borderColor);

    glNamedFramebufferTexture(gBufferFBO_, GL_DEPTH_ATTACHMENT, gDepth_, 0);

    GLenum status = glCheckNamedFramebufferStatus(gBufferFBO_, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        spdlog::error(status);
        throw runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
}