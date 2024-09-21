#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>

class Shader;

class Shadow
{
    uint32_t fbo_ = -1;
    uint32_t depth_texture_ = -1;
    uint32_t height_ = 1024, width_ = 1024;
    float near_plane_ = 1.f, far_plane_ = 20.f;
    glm::vec3 light_pos_;
    glm::mat4 light_space_matrix_;

    std::shared_ptr<Shader> shader_;

  public:
    Shadow(const std::shared_ptr<Shader> &shader, glm::vec3 light_pos, uint32_t width_ = 1024, uint32_t height_ = 1024);
    ~Shadow();
    Shadow(const Shadow &) = delete;
    Shadow &operator=(const Shadow &) = delete;
    Shadow(Shadow &&) = delete;
    Shadow &operator=(Shadow &&) = delete;

    void Init();
    void Use();

    [[nodiscard]] uint32_t GetDepthTexture() const
    {
        return depth_texture_;
    }

    [[nodiscard]] std::shared_ptr<Shader> GetShader() const
    {
        return shader_;
    }

    [[nodiscard]] glm::mat4 GetLightSpaceMatrix() const
    {
        return light_space_matrix_;
    }
};