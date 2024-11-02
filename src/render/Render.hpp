#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "Brdf.hpp"
#include "Camera/Camera.hpp"
#include "ConvolutionCubeMap.hpp"
#include "Equirectangular2Cubemap.hpp"
#include "Picking.hpp"
#include "Prefiltering.hpp"
#include "Primitive/Primitive.hpp"
#include "SSAO.hpp"
#include "Shader/Shader.hpp"
#include "Shadow/Shadow.hpp"
#include "render/Shadow/CascadedShadowMap.hpp"

struct UBOCamera
{
    glm::mat4 proj = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    alignas(16) glm::vec3 viewPos = glm::vec3(0.0f);
};
struct UBOEnviroment
{
    alignas(16) glm::vec3 sun_color = glm::vec3(0.0f, 0.0f, 0.0f);
    alignas(16) glm::vec3 sun_direction = glm::vec3(0.0f, 0.0f, 0.0f);
};

class Render
{
  public:
    static Render &GetInstance()
    {
        static Render instance;
        return instance;
    }
    ~Render();
    void Init();
    void Draw(const std::vector<std::shared_ptr<IPrimitive>> &primitives);

    void SetWidth(uint32_t width)
    {
        width_ = width;
    }
    void SetHeight(uint32_t height)
    {
        height_ = height;
    }

    [[nodiscard]] std::unordered_map<std::string, std::shared_ptr<Shader>> &GetShadersMap()
    {
        return shaders_map_;
    }

    void SetCamera(const std::shared_ptr<Camera> &camera)
    {
        camera_ = camera;
    }
    [[nodiscard]] std::shared_ptr<Camera> GetCamera() const
    {
        return camera_;
    }

    [[nodiscard]] std::unordered_map<std::string, std::shared_ptr<IMesh>> &GetMeshesMap()
    {
        return meshes_map_;
    }
    uint32_t GetTexture(const std::string &path);
    uint32_t GetTextureHDR(const std::string &path);

    [[nodiscard]] uint32_t GetEnvCubemap() const
    {
        return equirectangular_cubemap_->GetEnvCubemap();
    }
    [[nodiscard]] uint32_t GetConvolutionCubemap() const
    {
        return convolution_cubemap_->GetIrradianceMap();
    }
    [[nodiscard]] uint32_t GetPrefilterCubemap() const
    {
        return prefilter_cubemap_->GetPrefilterMap();
    }
    [[nodiscard]] uint32_t GetBRDF() const
    {
        return brdf_->GetBRDF();
    }

    int mouse_pos_x, mouse_pos_y;
    int mouse_pos_x_last, mouse_pos_y_last;

    void SetPicking(bool is_picking)
    {
        is_picking_ = is_picking;
    }

    bool GetIsSelected()
    {
        return is_selected_;
    }

    glm::vec3 GetTranslate()
    {
        // return translate_;
        return offset_;
    }

  private:
    Render();
    void CreateBasicGeometry();
    void CreatePlane();
    void CreateCube();
    void CreateSphere();
    void LoadShaders();
    void InitShaders();

    void CreateUBO();
    void UpdateUBO();

    void CreateDefaultTexture();
    void CreateMultisampledFramebuffer();
    void CreateShadowFramebuffer();
    void SetLightSpaceMatrix();
    void CreateTestFramebuffer();

    std::unordered_map<std::string, uint32_t> textures_map_;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_map_;
    std::unordered_map<std::string, std::shared_ptr<IMesh>> meshes_map_;

    std::shared_ptr<Camera> camera_;

    uint32_t ubo_camera_ = -1;
    uint32_t ubo_enviroment_ = -1;
    UBOCamera camera_ubo_data_;
    UBOEnviroment ubo_enviroment_data_;

    uint32_t cube_, light_cube_, shadow_cube_;

    uint32_t model_primitive_;

    // std::shared_ptr<Model<Vertex>> model_;
    // std::shared_ptr<ModelSkeletal> model_skeletal_;
    // std::shared_ptr<Animation> animation_;
    // std::shared_ptr<Animator> animator_;

    glm::vec3 light_position_ = glm::vec3(5.0f, 2.5f, 5.0f);
    glm::vec3 sun_direction_;
    glm::vec3 sun_color_;

    uint32_t width_ = -1, height_ = -1;
    uint32_t sample_count_ = 16;
    uint32_t multisampled_framebuffer_ = -1;
    uint32_t multisampled_color_buffer_ = -1;
    uint32_t multisampled_depth_buffer_ = -1;

    uint32_t shadow_framebuffer_ = -1;
    uint32_t shadow_texture_ = -1;
    uint32_t shadow_width_ = 4098, shadow_height_ = 4098;

    glm::mat4 lightSpaceMatrix_;

    std::shared_ptr<CascadedShadowMap> shadow_csm_;
    // std::shared_ptr<Shadow> shadow_;
    std::shared_ptr<Picking> picking_;

    std::thread compute_thread_;

    uint32_t gBufferFBO_ = -1;
    uint32_t gPosition_ = -1, gNormal_ = -1, gAlbedoSpec_ = -1;
    uint32_t gDepth_ = -1;

    std::shared_ptr<SSAO> ssao_;

    void DrawDepthMipmap();

    uint32_t test_fbo_ = -1;
    uint32_t test_dpeth_texture_ = -1;
    uint32_t test_color_texture = -1;

    std::shared_ptr<Equirectangular2Cubemap> equirectangular_cubemap_;
    std::shared_ptr<ConvolutionCubeMap> convolution_cubemap_;
    std::shared_ptr<Prefiltering> prefilter_cubemap_;
    std::shared_ptr<Brdf> brdf_;

    bool is_picking_ = false;
    bool is_selected_ = false;
    uint32_t selected_id_ = -1;
    glm::vec3 offset_;
    glm::mat4 translate_;
};