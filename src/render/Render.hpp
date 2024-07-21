#pragma once

#include <string>
#include <unordered_map>

#include "Primitive/Cube.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "assimp/Model.hpp"
#include "ShadowMap.hpp"

class Render
{
public:
    Render();
    ~Render();
    void Init(uint32_t width, uint32_t height);
    void Update();

    Camera camera_;

private:
    void LoadShaders();
    void LoadModel();
    void CreateDefaultTexture();
    void CreateMultisampledFramebuffer();
    void CreateShadowFramebuffer();
    void SetLightSpaceMatrix();

    std::unordered_map<std::string, uint32_t> textures_map_;
    std::unordered_map<uint32_t, std::shared_ptr<Primitive>> primitives_map_;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_map_;

    uint32_t cube_, light_cube_, shadow_cube_;

    uint32_t model_primitive_;

    std::shared_ptr<Model> model_;

    glm::vec3 light_position_ = glm::vec3(1.0f, 15.0f, 0.0f);
    glm::vec3 sun_direction_;

    uint32_t width_ = -1, height_ = -1;
    uint32_t sample_count_ = 16;
    uint32_t multisampled_framebuffer_ = -1;
    uint32_t multisampled_color_buffer_ = -1;
    uint32_t multisampled_depth_buffer_ = -1;

    uint32_t shadow_framebuffer_ = -1;
    uint32_t shadow_texture_ = -1;
    uint32_t shadow_width_ = 4098, shadow_height_ = 4098;

    glm::mat4 lightSpaceMatrix_;

    std::shared_ptr<ShadowMap> shadow_map_;
};