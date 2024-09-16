#include "Brdf.hpp"

#include "Mesh/IMesh.hpp"
#include "Render.hpp"
#include "Shader/Shader.hpp"
#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include <array>

using namespace std;

namespace
{
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
} // namespace

void Brdf::Init(uint32_t width, uint32_t height)
{
    shader_ = Render::GetInstance().GetShadersMap()["brdf"];
    width_ = width;
    height_ = height;

    glCreateFramebuffers(1, &fbo_);
    std::array<uint32_t, 1> attachments{GL_COLOR_ATTACHMENT0};
    glNamedFramebufferDrawBuffers(fbo_, attachments.size(), attachments.data());

    // depth
    glCreateTextures(GL_TEXTURE_2D, 1, &dpeth_texture_);
    glTextureStorage2D(dpeth_texture_, 1, GL_DEPTH_COMPONENT32, width_, height_);
    glTextureParameteri(dpeth_texture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(dpeth_texture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(dpeth_texture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(dpeth_texture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, dpeth_texture_, 0);

    glCreateTextures(GL_TEXTURE_2D, 1, &brdflut_texture_);
    glTextureStorage2D(brdflut_texture_, 1, GL_RG16F, width_, height_);
    glTextureParameteri(brdflut_texture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(brdflut_texture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(brdflut_texture_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(brdflut_texture_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum status = glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        spdlog::error(status);
        throw runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    Draw();
}

void Brdf::Draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0, brdflut_texture_, 0);
    glViewport(0, 0, width_, height_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_->use();
    renderQuad();
}
