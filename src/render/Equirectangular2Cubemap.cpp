#include "Equirectangular2Cubemap.hpp"

#include "Shader/Shader.hpp"

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "spdlog/spdlog.h"

#include <array>

using namespace std;

void Equirectangular2Cubemap::Init(const std::shared_ptr<Shader> &equirectangularToCubemapShader, uint32_t hdrTexture,
                                   uint32_t width, uint32_t height, const std::shared_ptr<IMesh> &cubeMesh)
{
    equirectangularToCubemapShader_ = equirectangularToCubemapShader;
    hdrTexture_ = hdrTexture;
    width_ = width;
    height_ = height;
    cubeMesh_ = cubeMesh;

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

    // 6 faces
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &envCubemap_);
    glTextureStorage2D(envCubemap_, 1, GL_RGB16F, width_, height_);
    glTextureParameteri(envCubemap_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(envCubemap_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(envCubemap_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(envCubemap_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(envCubemap_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum status = glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        spdlog::error(status);
        throw runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    Draw();
}

void Equirectangular2Cubemap::Draw()
{
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

    // convert HDR equirectangular environment map to cubemap equivalent
    equirectangularToCubemapShader_->use();
    equirectangularToCubemapShader_->setMat4("projection", captureProjection);

    glBindTextureUnit(0, hdrTexture_);

    glViewport(0, 0, width_, height_); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader_->setMat4("view", captureViews[i]);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap,
        // 0);
        glNamedFramebufferTextureLayer(fbo_, GL_COLOR_ATTACHMENT0, envCubemap_, 0, i);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(cubeMesh_->GetVAO());
        glDrawElements(GL_TRIANGLES, cubeMesh_->GetIndicesCount(), GL_UNSIGNED_INT, 0);
        // renderCube(); // renders a 1x1 cube
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}