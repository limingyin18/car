#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "render/Camera/Camera.hpp"

class Shader;

class CascadedShadowMap
{
  public:
    void Init(const Camera &_camera, const glm::vec3 &_lightDir, const std::shared_ptr<Shader> &_shader,
              const std::shared_ptr<Shader> &_shader_skeletal);
    void Use(const Camera &_camera);
    [[nodiscard]] std::shared_ptr<Shader> GetShader() const
    {
        return shader_;
    }
    [[nodiscard]] std::shared_ptr<Shader> GetShaderSkeletal() const
    {
        return shader_skeletal_;
    }

    [[nodiscard]] const std::vector<float> &GetShadowCascadeLevels() const
    {
        return shadowCascadeLevels_;
    }
    [[nodiscard]] uint32_t GetDepthMap() const
    {
        return lightDepthMaps_;
    }

  private:
    void CreateFBO();
    uint32_t lightFBO_;
    uint32_t lightDepthMaps_;
    uint32_t depthMapResolution_ = 4098;
    uint32_t matricesUBO_;

    std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view)
    {
        return GetFrustumCornersWorldSpace(proj * view);
    }
    std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4 &projview);
    glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane);
    std::vector<glm::mat4> GetLightSpaceMatrices();

    std::vector<float> shadowCascadeLevels_;

    glm::mat4 lightSpaceMatrix_;
    glm::mat4 lightView_;
    glm::mat4 lightProj_;
    glm::mat4 cameraProj_;
    glm::mat4 cameraView_;
    glm::vec3 lightDir_;
    float cameraNearPlane_, cameraFarPlane_;

    std::shared_ptr<Shader> shader_;
    std::shared_ptr<Shader> shader_skeletal_;

    uint32_t fb_width_, fb_height_;
    float zoom_;
};