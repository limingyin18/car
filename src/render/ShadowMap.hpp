#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Camera.hpp"
#include "Shader.hpp"

class ShadowMap
{
public:
    ShadowMap();
    ~ShadowMap();

    void Init(const Camera& _camera, const glm::vec3 &_lightDir, std::shared_ptr<Shader> _shader);
    void Use(const Camera& _camera);

public:
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

    uint32_t fb_width_, fb_height_;
    float zoom_;
};