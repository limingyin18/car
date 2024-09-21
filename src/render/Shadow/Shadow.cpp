#include "Shadow.hpp"

#include "render/Shader/Shader.hpp"

#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

using namespace std;

Shadow::Shadow(const std::shared_ptr<Shader> &shader, glm::vec3 light_pos, uint32_t width, uint32_t height)
    : shader_(shader), light_pos_(light_pos), width_(width), height_(height)
{
}

Shadow::~Shadow()
{
    glDeleteFramebuffers(1, &fbo_);
    glDeleteTextures(1, &depth_texture_);
}

void Shadow::Init()
{
    glCreateFramebuffers(1, &fbo_);

    glCreateTextures(GL_TEXTURE_2D, 1, &depth_texture_);
    glTextureStorage2D(depth_texture_, 1, GL_DEPTH_COMPONENT32F, width_, height_);
    glTextureParameteri(depth_texture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(depth_texture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(depth_texture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(depth_texture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTextureParameterfv(depth_texture_, GL_TEXTURE_BORDER_COLOR, border_color);

    glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, depth_texture_, 0);
    glNamedFramebufferDrawBuffer(fbo_, GL_NONE);
    glNamedFramebufferReadBuffer(fbo_, GL_NONE);

    GLenum status = glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        spdlog::error("{}, {}, {}, {}", __FILE__, __FUNCTION__, __LINE__, status);
        throw runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    light_space_matrix_ = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane_, far_plane_) *
                          glm::lookAt(light_pos_, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
}

void Shadow::Use()
{
    glViewport(0, 0, width_, height_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClear(GL_DEPTH_BUFFER_BIT);

    shader_->use();
    shader_->setMat4("lightSpaceMatrix", light_space_matrix_);

    glBindTextureUnit(6, depth_texture_);
}