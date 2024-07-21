#include "ShadowMap.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <spdlog/spdlog.h>

#include <glad/glad.h>

ShadowMap::ShadowMap()
{
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::CreateFBO()
{
    // configure light FBO
    // -----------------------
    glGenFramebuffers(1, &lightFBO_);

    glGenTextures(1, &lightDepthMaps_);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps_);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution_, depthMapResolution_, int(shadowCascadeLevels_.size()) + 1,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO_);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightDepthMaps_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        spdlog::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure UBO
    // --------------------
    glGenBuffers(1, &matricesUBO_);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShadowMap::Init(const Camera &_camera, const glm::vec3 &_lightDir, std::shared_ptr<Shader> _shader)
{
    shader_ = _shader;

    zoom_ = _camera.GetFov();
    fb_height_ = _camera.GetHeight();
    fb_width_ = _camera.GetWidth();

    cameraFarPlane_ = _camera.GetFar();
    cameraNearPlane_ = _camera.GetNear();
    shadowCascadeLevels_ = std::vector<float>{cameraFarPlane_ / 50.0f, cameraFarPlane_ / 25.0f, cameraFarPlane_ / 10.0f, cameraFarPlane_ / 2.0f};

    cameraProj_ = _camera.GetProj();
    cameraView_ = _camera.GetView();

    lightDir_ = _lightDir;

    CreateFBO();
}

void ShadowMap::Use(const Camera &_camera)
{
    cameraProj_ = _camera.GetProj();
    cameraView_ = _camera.GetView();

    const auto lightMatrices = GetLightSpaceMatrices();
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO_);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shader_->use();
    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO_);
    glViewport(0, 0, depthMapResolution_, depthMapResolution_);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT); // peter panning
    // renderScene(simpleDepthShader);
    // glCullFace(GL_BACK);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::vector<glm::vec4> ShadowMap::GetFrustumCornersWorldSpace(const glm::mat4 &projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt =
                    inv * glm::vec4(
                              2.0f * x - 1.0f,
                              2.0f * y - 1.0f,
                              2.0f * z - 1.0f,
                              1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

glm::mat4 ShadowMap::GetLightSpaceMatrix(const float nearPlane, const float farPlane)
{
    const auto proj = glm::perspective(
        glm::radians(zoom_), (float)fb_width_ / (float)fb_height_, nearPlane,
        farPlane);
    const auto corners = GetFrustumCornersWorldSpace(proj, cameraView_);

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto &v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center + lightDir_, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto &v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    auto temp = -minZ;
    minZ = -maxZ;
    maxZ = temp;
    auto mid = (maxZ - minZ) / 2;
    minZ -= mid * 10.0f;
    maxZ += mid * 10.0f;

    // // Tune this parameter according to the scene
    // constexpr float zMult = 10.f;
    // if (minZ < 0)
    // {
    //     minZ *= zMult;
    // }
    // else
    // {
    //     minZ /= zMult;
    // }
    // if (maxZ < 0)
    // {
    //     maxZ /= zMult;
    // }
    // else
    // {
    //     maxZ *= zMult;
    // }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

std::vector<glm::mat4> ShadowMap::GetLightSpaceMatrices()
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels_.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(GetLightSpaceMatrix(cameraNearPlane_, shadowCascadeLevels_[i]));
        }
        else if (i < shadowCascadeLevels_.size())
        {
            ret.push_back(GetLightSpaceMatrix(shadowCascadeLevels_[i - 1], shadowCascadeLevels_[i]));
        }
        else
        {
            ret.push_back(GetLightSpaceMatrix(shadowCascadeLevels_[i - 1], cameraFarPlane_));
        }
    }
    return ret;
}