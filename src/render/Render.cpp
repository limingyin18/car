#include "Render.hpp"

#include "BasicGeometry/Sphere.hpp"
#include "ConvolutionCubeMap.hpp"
#include "Prefiltering.hpp"
#include "Primitive/PrimitiveIndirect.hpp"
#include "Primitive/PrimitiveInstance.hpp"
#include "Primitive/PrimitiveOcean.hpp"
#include "Primitive/PrimitiveSkeletalIndirect.hpp"
#include "Shadow/CascadedShadowMap.hpp"
#include "glad/glad.h"
#include "render/Brdf.hpp"
#include "tools/Tool.hpp"

#include <cstdint>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <memory>
#include <regex>

using namespace std;

void renderQuad();

namespace
{
void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message,
                      void const *user_param)
{
    auto const src_str = [source]() {
        switch (source)
        {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
        default:
            return "UNKNOWN";
        }
    }();

    auto const type_str = [type]() {
        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        default:
            return "UNKNOWN";
        }
    }();

    auto const severity_str = [severity]() {
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        default:
            return "UNKNOWN";
        }
    }();
    std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}
} // namespace

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
    spdlog::debug("LoadShaders");
    // regex match filename with .vert
    regex cs_filesname_rule(".*comp$");
    for (auto &entry : filesystem::directory_iterator("shaders/cs"))
    {
        string shader_name;
        string cs_shader_file_name;
        if (regex_match(entry.path().string(), cs_filesname_rule))
        {
            shader_name = entry.path().stem().string();
            cs_shader_file_name = entry.path().string();
            shaders_map_[shader_name] = make_shared<Shader>();
            shaders_map_[shader_name]->Init(cs_shader_file_name.c_str());
        }
    }

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

void Render::Init()
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(message_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    CreateMultisampledFramebuffer();

    LoadShaders();
    CreateDefaultTexture();
    CreateBasicGeometry();

    // light_position_ = glm::vec3(10.0f, 100.0f, 10.0f);
    sun_direction_ = glm::normalize(light_position_ - glm::vec3(0.0f, 0.0f, 0.0f));
    sun_color_ = glm::vec3(255, 255, 255) / 255.f;

    camera_ = make_shared<Camera>();
    camera_->SetWidth(width_);
    camera_->SetHeight(height_);
    camera_->SetPosition(glm::vec3(0.0f, 5.0f, 5.0f));
    camera_->SetFront(glm::vec3(0.f, 0.f, 0.f) - camera_->GetPosition());
    camera_->LookAt();
    camera_->Proj();

    shadow_csm_ = make_shared<CascadedShadowMap>();
    shadow_csm_->Init(*camera_, sun_direction_, shaders_map_["cascade_shadow"],
                      shaders_map_["cascade_shadow_skeletal"]);

    // shadow_ = make_shared<Shadow>(shaders_map_["shadow"], light_position_);
    // shadow_->Init();

    picking_ = make_shared<Picking>();
    picking_->Init(shaders_map_["picking_instance"]);

    InitShaders();
    CreateUBO();

    equirectangular_cubemap_ = make_shared<Equirectangular2Cubemap>();
    equirectangular_cubemap_->Init(shaders_map_["equirectangular2cubemap"],
                                   GetTextureHDR("assets/textures/hdr/industrial_sunset_puresky_2k.hdr"), 512, 512,
                                   meshes_map_["cube"]);

    convolution_cubemap_ = make_shared<ConvolutionCubeMap>();
    convolution_cubemap_->Init(shaders_map_["convolution_cubemap"], equirectangular_cubemap_->GetEnvCubemap(),
                               meshes_map_["cube"]);

    prefilter_cubemap_ = make_shared<Prefiltering>();
    prefilter_cubemap_->Init(shaders_map_["prefiltering"], equirectangular_cubemap_->GetEnvCubemap(),
                             meshes_map_["cube"]);

    brdf_ = make_shared<Brdf>();
    brdf_->Init();

    spdlog::debug("Render Init finish");
}

void Render::Draw(const std::vector<std::shared_ptr<IPrimitive>> &primitives)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera_->LookAt();
    UpdateUBO();

    for (auto &primitive : primitives)
    {
        auto primitives_indirect = dynamic_pointer_cast<PrimitiveIndirect>(primitive);
        if (primitives_indirect != nullptr)
        {
            primitives_indirect->SetCamera(camera_);
        }
    }

    // // picking pass
    // if (is_picking_)
    // {
    //     picking_->Use(mouse_pos_x, height_ - 1 - mouse_pos_y);

    //     for (uint32_t i = 0; i < primitives.size(); ++i)
    //     {
    //         auto &primitive = primitives[i];
    //         if (dynamic_pointer_cast<Sphere>(primitive->GetMesh()) == nullptr)
    //         {
    //             continue;
    //         }
    //         picking_->GetShader()->setInt("actorID", i);
    //         auto shader = primitive->GetShader();
    //         primitive->SetShader(picking_->GetShader());
    //         primitive->Draw();
    //         primitive->SetShader(shader);
    //     }
    //     glDisable(GL_SCISSOR_TEST);
    //     uint32_t index = picking_->Pick();
    //     if (index != -1)
    //     {
    //         selected_id_ = index;
    //         is_selected_ = true;

    //         spdlog::debug("selected {}", selected_id_);
    //     }
    //     else
    //     {
    //         is_selected_ = false;
    //         spdlog::debug("not selected");
    //     }
    //     is_picking_ = false;
    // }

    // // shadow pass
    // {
    //     // shadow_map_->Use(*camera_);
    //     // auto light_space_matrix = shadow_->GetLightSpaceMatrix();
    //     shadow_csm_->Use(*camera_);

    //     for (auto &primitive : primitives)
    //     {
    //         if (std::dynamic_pointer_cast<PrimitiveSkeletalIndirect>(primitive) != nullptr)
    //         {
    //             auto shader = primitive->GetShader();
    //             primitive->SetShader(shadow_csm_->GetShaderSkeletal());
    //             primitive->Draw();
    //             primitive->SetShader(shader);
    //         }
    //     }

    //     for (auto &primitive : primitives)
    //     {
    //         auto shader = primitive->GetShader();
    //         if (shader == shaders_map_["skybox"])
    //         {
    //             continue;
    //         }

    //         if (std::dynamic_pointer_cast<PrimitiveSkeletalIndirect>(primitive) != nullptr)
    //         {
    //             continue;
    //         }

    //         if (std::dynamic_pointer_cast<PrimitiveOcean>(primitive) != nullptr)
    //         {
    //             continue;
    //         }
    //         primitive->SetShader(shadow_csm_->GetShader());
    //         // primitive->Draw();
    //         primitive->SetShader(shader);
    //         // shader->use();
    //         // shader->setMat4("lightSpaceMatrix", light_space_matrix);
    //     }
    // }

    // normal pass
    {
        glBindFramebuffer(GL_FRAMEBUFFER, multisampled_framebuffer_);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
        glViewport(0, 0, width_, height_);
        glDisable(GL_CULL_FACE);

        uint32_t depth_texture = shadow_csm_->GetDepthMap();
        glBindTextureUnit(6, depth_texture);
        for (auto &primitive : primitives)
        {
            // glEnable(GL_CULL_FACE);
            primitive->Draw();
        }
    }

    // if (is_selected_)
    // {
    //     // high light
    //     auto highlight_shader = shaders_map_["highlight"];
    //     for (uint32_t i = 0; i < primitives.size(); ++i)
    //     {
    //         auto &primitive = primitives[i];
    //         if (dynamic_pointer_cast<Sphere>(primitive->GetMesh()) == nullptr)
    //         {
    //             continue;
    //         }

    //         float dx = float(mouse_pos_x - mouse_pos_x_last) / width_ * 2.f;
    //         float dy = float(mouse_pos_y - mouse_pos_y_last) / height_ * 2.f;
    //         glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(dx, 0.f, dy));

    //         auto primitive_inst = dynamic_pointer_cast<PrimitiveInstance>(primitive);

    //         highlight_shader->use();
    //         auto model_matrix = primitive_inst->GetInstanceTransforms()[selected_id_];
    //         model_matrix = glm::scale(model_matrix, glm::vec3(1.1f));
    //         model_matrix = translate * model_matrix;
    //         highlight_shader->setMat4("model", model_matrix);
    //         primitive->SetTransform(model_matrix);
    //         auto shader = primitive->GetShader();
    //         primitive->SetShader(highlight_shader);
    //         primitive->Primitive::Draw();
    //         primitive->SetShader(shader);
    //         primitive->SetTransform(glm::mat4(1.f));
    //     }
    // }

    // copy to window
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampled_framebuffer_);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}

void Render::InitShaders()
{
    auto &shadowCascadeLevels = shadow_csm_->GetShadowCascadeLevels();
    spdlog::debug("shadow cascade levels: {}", shadowCascadeLevels.size());
    shaders_map_["pbr"]->use();
    shaders_map_["pbr"]->setInt("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
    {
        shaders_map_["pbr"]->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }

    shaders_map_["phong"]->use();
    shaders_map_["phong"]->setInt("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
    {
        shaders_map_["phong"]->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }
}

void Render::CreateUBO()
{
    glCreateBuffers(1, &ubo_camera_);
    camera_ubo_data_.proj = camera_->GetProj();
    camera_ubo_data_.view = camera_->GetView();
    camera_ubo_data_.viewPos = camera_->GetPosition();
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
    camera_ubo_data_.view = camera_->GetView();
    camera_ubo_data_.viewPos = camera_->GetPosition();
    glNamedBufferSubData(ubo_camera_, 0, sizeof(UBOCamera), &camera_ubo_data_);
}

uint32_t Render::GetTexture(const std::string &path)
{
    auto it = textures_map_.find(path);
    if (it != textures_map_.end())
    {
        return it->second;
    }
    else
    {
        textures_map_[path] = Tool::LoadTexture(path);
        return textures_map_[path];
    }
}

uint32_t Render::GetTextureHDR(const std::string &path)
{
    auto it = textures_map_.find(path);
    if (it != textures_map_.end())
    {
        return it->second;
    }
    else
    {
        textures_map_[path] = Tool::LoadTextureHDR(path);
        return textures_map_[path];
    }
}