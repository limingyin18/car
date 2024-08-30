#include "Render.hpp"

#include "assimp/ModelSkeletal.hpp"
#include "glad/glad.h"

#include "assimp/Model.hpp"
#include "render/Primitive/Cube.hpp"
#include <spdlog/spdlog.h>

#include <filesystem>
#include <regex>

using namespace std;

void renderQuad();

Render::Render()
{
}

Render::~Render()
{
    for (auto &shader : shaders_map_)
    {
        glDeleteProgram(shader.second->ID);
    }
    shaders_map_.clear();

    for (auto &texture : textures_map_)
    {
        glDeleteTextures(1, &texture.second);
    }
    textures_map_.clear();

    // glDeleteFramebuffers(1, &multisampled_framebuffer_);
    glDeleteTextures(1, &multisampled_color_buffer_);
    glFinish();
    // glDeleteRenderbuffers(1, &multisampled_depth_buffer_);

    spdlog::debug("render destruct");
}

void Render::LoadShaders()
{
    // regex match filename with .vert
    regex vs_filesname_rule(".*vert$");
    for (auto &entry : filesystem::directory_iterator("shaders/vs_fs"))
    {
        string shader_name;
        string vs_shader_file_name;
        string fs_shader_file_name;
        if (regex_match(entry.path().string(), vs_filesname_rule))
        {
            shader_name = entry.path().stem().string();
            vs_shader_file_name = entry.path().string();
            auto fs_path = entry.path();
            fs_shader_file_name = fs_path.replace_extension(".frag").string();
            shaders_map_[shader_name] = make_shared<Shader>();
            shaders_map_[shader_name]->Init(vs_shader_file_name.c_str(), fs_shader_file_name.c_str());
        }
    }

    for (auto &entry : filesystem::directory_iterator("shaders/vs_gs_fs"))
    {
        string shader_name;
        string vs_shader_file_name;
        string fs_shader_file_name;
        string gs_shader_file_name;
        if (regex_match(entry.path().string(), vs_filesname_rule))
        {
            shader_name = entry.path().stem().string();
            vs_shader_file_name = entry.path().string();
            auto fs_path = entry.path();
            fs_shader_file_name = fs_path.replace_extension(".frag").string();
            gs_shader_file_name = fs_path.replace_extension(".geom").string();
            shaders_map_[shader_name] = make_shared<Shader>();
            shaders_map_[shader_name]->Init(vs_shader_file_name.c_str(), gs_shader_file_name.c_str(),
                                            fs_shader_file_name.c_str());
        }
    }
}

void Render::Init(uint32_t width, uint32_t height)
{
    width_ = width;
    height_ = height;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    CreateMultisampledFramebuffer();

    LoadShaders();
    CreateDefaultTexture();
    CreateCube();

    sun_direction_ = glm::normalize(light_position_ - glm::vec3(0.0f, 0.0f, 0.0f));
    sun_color_ = glm::vec3(1.0f, 1.0f, 1.0f);

    camera_.SetWidth(width);
    camera_.SetHeight(height);
    camera_.SetPosition(glm::vec3(0.0f, 5.0f, 5.0f));
    camera_.SetFront(glm::vec3(0.f, 0.f, 0.f) - camera_.GetPosition());
    camera_.LookAt();
    camera_.Proj();

    InitShaders();
    CreateUBO();
}

void Render::Draw(const std::vector<std::shared_ptr<Primitive>> &primitives)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera_.LookAt();
    UpdateUBO();

    glBindFramebuffer(GL_FRAMEBUFFER, multisampled_framebuffer_);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glViewport(0, 0, width_, height_);

    for (auto &primitive : primitives)
    {
        primitive->Draw();
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampled_framebuffer_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Render::InitShaders()
{
    // shaders_map_["skeletal_phong"]->setInt("shadowMap", 1);
    // shaders_map_["skeletal_phong"]->setFloat("farPlane", camera_.GetFar());
    // shaders_map_["skeletal_phong"]->setInt("cascadeCount", shadow_map_->shadowCascadeLevels_.size());
    // for (size_t i = 0; i < shadow_map_->shadowCascadeLevels_.size(); ++i)
    // {
    //     shaders_map_["skeletal_phong"]->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]",
    //                                              shadow_map_->shadowCascadeLevels_[i]);
    // }
}

void Render::CreateUBO()
{
    glCreateBuffers(1, &ubo_camera_);
    camera_ubo_data_.proj = camera_.GetProj();
    camera_ubo_data_.view = camera_.GetView();
    camera_ubo_data_.viewPos = camera_.GetPosition();
    glNamedBufferStorage(ubo_camera_, sizeof(UBOCamera), &camera_ubo_data_, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_camera_);

    glCreateBuffers(1, &ubo_enviroment_);
    ubo_enviroment_data_.sun_color = sun_color_;
    ubo_enviroment_data_.sun_direction = sun_direction_;
    glNamedBufferStorage(ubo_enviroment_, sizeof(UBOEnviroment), &ubo_enviroment_data_, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_enviroment_);
}

void Render::UpdateUBO()
{
    camera_ubo_data_.view = camera_.GetView();
    camera_ubo_data_.viewPos = camera_.GetPosition();
    glNamedBufferSubData(ubo_camera_, 0, sizeof(UBOCamera), &camera_ubo_data_);
}

void Render::CreateCube()
{
    auto cube = make_shared<Cube>();
    cube->Init(textures_map_["default_red"]);
    meshes_map_["cube"] = cube;
}

void Render::CreateDefaultTexture()
{
    {
        uint32_t texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_RGBA8, 1, 1);
        uint32_t color = 0xffffffff;
        glTextureSubImage2D(texture, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);
        textures_map_["default_white"] = texture;
    }

    {
        uint32_t texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_RGBA8, 1, 1);
        uint32_t color = 0xff0000ff;
        glTextureSubImage2D(texture, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);
        textures_map_["default_red"] = texture;
    }
}