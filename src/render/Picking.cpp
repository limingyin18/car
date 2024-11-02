#include "Picking.hpp"

#include "Shader/Shader.hpp"

#include "glad/glad.h"

#include <array>
#include <cstdint>

using namespace std;

void Picking::Init(const std::shared_ptr<Shader> &shader)
{
    shader_ = shader;
    width_ = 1280;
    height_ = 960;

    glCreateFramebuffers(1, &fbo_);

    glCreateTextures(GL_TEXTURE_2D, 1, &texture_index_);
    glTextureStorage2D(texture_index_, 1, GL_R16F, width_, height_);
    glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0, texture_index_, 0);
    std::array<uint32_t, 1> attachments{GL_COLOR_ATTACHMENT0};
    glNamedFramebufferDrawBuffers(fbo_, attachments.size(), attachments.data());

    glCreateTextures(GL_TEXTURE_2D, 1, &texture_depth_);
    glTextureStorage2D(texture_depth_, 1, GL_DEPTH_COMPONENT32F, width_, height_);
    glTextureParameteri(texture_depth_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texture_depth_, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTextureParameteri(texture_depth_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture_depth_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, texture_depth_, 0);

    GLenum status = glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        throw runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
}

void Picking::Use(int x_offset, int y_offset)
{
    x_offset_ = x_offset;
    y_offset_ = y_offset;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glClearColor(-1.f, -1.f, -1.f, 1.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glScissor(x_offset_, y_offset_, 1, 1);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width_, height_);
    shader_->use();
}

uint32_t Picking::Pick()
{
    float index = 0.f;
    glReadPixels(x_offset_, y_offset_, 1, 1, GL_RED, GL_FLOAT, &index);
    index = std::round(index);

    glReadPixels(x_offset_, y_offset_, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth_);
    return index;
}