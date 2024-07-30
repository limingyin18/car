#include "Render.hpp"

#include "glad/glad.h"

#include "assimp/Model.hpp"
#include <spdlog/spdlog.h>

#include <regex>
#include <filesystem>

using namespace std;

void renderQuad();

Render::Render()
{
    LoadShaders();
}

Render::~Render()
{
    model_ = nullptr;

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

    for (auto &primitive : primitives_map_)
    {
        primitive.second = nullptr;
    }
    primitives_map_.clear();

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
            shaders_map_[shader_name]->Init(vs_shader_file_name.c_str(), gs_shader_file_name.c_str(), fs_shader_file_name.c_str());
        }
    }
}

void Render::CreateDefaultTexture()
{
    glGenTextures(1, &textures_map_["default_white"]);
    glBindTexture(GL_TEXTURE_2D, textures_map_["default_white"]);
    unsigned char *data = new unsigned char[3]{255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    delete[] data;

    glGenTextures(1, &textures_map_["default_orange"]);
    glBindTexture(GL_TEXTURE_2D, textures_map_["default_orange"]);

    glm::vec3 orange_color = glm::vec3(1.0f, 0.5f, 0.31f) * 255.0f;
    glm::u8vec3 orange_data = {(unsigned char)orange_color.r, (unsigned char)orange_color.g, (unsigned char)orange_color.b};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, glm::value_ptr(orange_data));
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Render::Init(uint32_t width, uint32_t height)
{
    width_ = width;
    height_ = height;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    CreateMultisampledFramebuffer();
    CreateShadowFramebuffer();
    CreateGBuffer();

    LoadShaders();
    sun_direction_ = glm::normalize(light_position_ - glm::vec3(0.0f, 0.0f, 0.0f));
    sun_color_ = glm::vec3(1.0f, 1.0f, 1.0f);
    CreateDefaultTexture();

    LoadModel();

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    camera_.SetWidth(width);
    camera_.SetHeight(height);
    camera_.SetPosition(glm::vec3(0.0f, 5.0f, 5.0f));
    camera_.SetFront(glm::vec3(0.f, 0.f, 0.f) - camera_.GetPosition());
    camera_.LookAt();
    camera_.Proj();

    shadow_map_ = make_shared<ShadowMap>();
    shadow_map_->Init(camera_, sun_direction_, shaders_map_["shadow"]);

    ssao_ = make_shared<SSAO>();
    ssao_->Init(shaders_map_["ssao"], shaders_map_["ssao_blur"], camera_);

    shaders_map_["ssao_lighting"]->use();
    shaders_map_["ssao_lighting"]->setVec3("lightColor", sun_color_);
    shaders_map_["ssao_lighting"]->setFloat("far_plane", camera_.GetFar());
    shaders_map_["ssao_lighting"]->setMat4("proj", camera_.GetProj());
    shaders_map_["ssao_lighting"]->setMat4("width", camera_.GetWidth());
    shaders_map_["ssao_lighting"]->setMat4("height", camera_.GetHeight());
    auto kernel = ssao_->GetKernel();
    for (unsigned int i = 0; i < 64; ++i)
        shaders_map_["ssao_lighting"]->setVec3("samples[" + std::to_string(i) + "]", kernel[i]);

    shaders_map_["gBuffer"]->use();
    shaders_map_["gBuffer"]->setMat4("view", camera_.GetView());
    shaders_map_["gBuffer"]->setMat4("projection", camera_.GetProj());

    shaders_map_["ssao_geometry"]->use();
    shaders_map_["ssao_geometry"]->setMat4("view", camera_.GetView());
    shaders_map_["ssao_geometry"]->setMat4("projection", camera_.GetProj());

    shaders_map_["deferred"]->use();
    shaders_map_["deferred"]->setVec3("lightColor", glm::vec3{1.0f, 1.0f, 1.0f});
    shaders_map_["deferred"]->setVec3("lightDir", sun_direction_);
    shaders_map_["deferred"]->setVec3("viewPos", camera_.GetPosition());

    shaders_map_["basic"]->use();
    shaders_map_["basic"]->setMat4("view", camera_.GetView());
    shaders_map_["basic"]->setMat4("projection", camera_.GetProj());

    shaders_map_["phong"]->use();
    shaders_map_["phong"]->setMat4("view", camera_.GetView());
    shaders_map_["phong"]->setMat4("projection", camera_.GetProj());
    shaders_map_["phong"]->setVec3("lightColor", glm::vec3{1.0f, 1.0f, 1.0f});
    // shaders_map_["phong"]->setVec3("lightPos", light_position);
    shaders_map_["phong"]->setVec3("lightDir", sun_direction_);
    shaders_map_["phong"]->setVec3("viewPos", camera_.GetPosition());
    shaders_map_["phong"]->setInt("shadowMap", 1);
    shaders_map_["phong"]->setFloat("farPlane", camera_.GetFar());
    shaders_map_["phong"]->setInt("cascadeCount", shadow_map_->shadowCascadeLevels_.size());
    for (size_t i = 0; i < shadow_map_->shadowCascadeLevels_.size(); ++i)
    {
        shaders_map_["phong"]->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadow_map_->shadowCascadeLevels_[i]);
    }

    glBindTexture(GL_TEXTURE_2D, textures_map_["default_white"]);

    cube_ = 0;
    light_cube_ = 1;
    shadow_cube_ = 2;
    primitives_map_[cube_] = make_shared<Cube>();
    primitives_map_[light_cube_] = make_shared<Cube>();
    primitives_map_[shadow_cube_] = make_shared<Cube>();

    {
        auto primitive = static_pointer_cast<Cube>(primitives_map_[cube_]);
        primitive->color_ = glm::vec3(1.0f, 0.5f, 0.31f);
        primitive->Init();
        primitive->GetMesh().GetTextures().push_back(Texture{textures_map_["default_orange"], "texture_diffuse", ""});
    }

    {
        auto primitive = static_pointer_cast<Cube>(primitives_map_[light_cube_]);

        primitive->color_ = glm::vec3(1.f, 1.f, 1.f);
        primitive->Init();
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(2.f, 2.f, -1.f));
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
        glm::mat4 light_model = translation * scale;
        primitive->SetModel(light_model);

        primitive->GetMesh().GetTextures().push_back(Texture{textures_map_["default_white"], "texture_diffuse", ""});
    }

    {
        auto primitive = static_pointer_cast<Cube>(primitives_map_[shadow_cube_]);

        primitive->color_ = glm::vec3(1.f, 1.f, 1.f);
        primitive->Init();
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), light_position_);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
        glm::mat4 light_model = translation * scale;
        primitive->SetModel(light_model);

        primitive->GetMesh().GetTextures().push_back(Texture{textures_map_["default_white"], "texture_diffuse", ""});
    }

    {
        model_primitive_ = 3;
        auto &mesh = model_->GetMeshes();
        for (int i = 0; i < mesh.size(); i++)
        {
            glm::mat4 model = mesh[i]->GetModel();
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            mesh[i]->SetModel(model);
            primitives_map_[model_primitive_ + i] = make_shared<Primitive>();
            auto primitive = primitives_map_[model_primitive_ + i];
            primitive->Init();
            primitive->SetMesh(mesh[i]);
        }
    }
}

void Render::Update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera_.LookAt();

    shaders_map_["ssao_geometry"]->use();
    shaders_map_["ssao_geometry"]->setMat4("view", camera_.GetView());
    DrawGBuffer();

    glBindTextureUnit(0, gPosition_);
    glBindTextureUnit(1, gNormal_);
    ssao_->Use();
    renderQuad();

    ssao_->UseBlur();
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTextureUnit(0, gPosition_);
    glBindTextureUnit(1, gNormal_);
    glBindTextureUnit(2, gAlbedoSpec_);
    glBindTextureUnit(3, ssao_->GetBlurColorTexture());
    glBindTextureUnit(4, ssao_->GetNoiseTexture());
    shaders_map_["ssao_lighting"]->use();
    glm::mat4 view = camera_.GetView();
    glm::mat4 normal_matrix = transpose(inverse(glm::mat3(view)));
    glm::vec4 sun_dir_4 = normal_matrix * glm::vec4(sun_direction_, 1.0);
    glm::vec3 lightDirView = glm::normalize(glm::vec3(sun_dir_4));
    shaders_map_["ssao_lighting"]->setVec3("lightDir", lightDirView);
    shaders_map_["ssao_lighting"]->setMat4("view", view);
    shaders_map_["ssao_lighting"]->setMat4("invView", glm::inverse(view));
    renderQuad();

    return;

    shaders_map_["basic"]->use();
    shaders_map_["basic"]->setMat4("view", camera_.GetView());

    shaders_map_["phong"]->use();
    shaders_map_["phong"]->setMat4("view", camera_.GetView());
    shaders_map_["phong"]->setVec3("viewPos", camera_.GetPosition());

    // glBindFramebuffer(GL_FRAMEBUFFER, shadow_framebuffer_);
    // glClear(GL_DEPTH_BUFFER_BIT);
    // glCullFace(GL_FRONT);
    // glViewport(0, 0, shadow_width_, shadow_height_);
    shadow_map_->Use(camera_);
    for (auto &primitive : primitives_map_)
    {
        primitive.second->Draw(shaders_map_["shadow"]);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, multisampled_framebuffer_);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glViewport(0, 0, width_, height_);

    shaders_map_["phong"]->use();
    // glBindTextureUnit(1, shadow_texture_);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_map_->lightDepthMaps_);
    for (auto &primitive : primitives_map_)
    {
        primitive.second->Draw(shaders_map_["phong"]);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampled_framebuffer_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Render::LoadModel()
{
    model_ = make_shared<Model>();
    model_->Load("assets/Sponza/glTF/Sponza.gltf");
}

void Render::SetLightSpaceMatrix()
{
    glm::mat4 lightProjection, lightView;
    float near_plane = 1.f, far_plane = 20.f;
    lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, near_plane, far_plane);
    lightView = glm::lookAt(light_position_,
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix_ = lightProjection * lightView;
    shaders_map_["shadow"]->use();
    shaders_map_["shadow"]->setMat4("lightSpaceMatrix", lightSpaceMatrix_);
    shaders_map_["phong"]->use();
    shaders_map_["phong"]->setMat4("lightSpaceMatrix", lightSpaceMatrix_);
}

void Render::DrawGBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &primitive : primitives_map_)
    {
        primitive.second->Draw(shaders_map_["ssao_geometry"]);
    }
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
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