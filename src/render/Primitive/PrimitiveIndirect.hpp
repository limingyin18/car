#pragma once

#include "../Camera/Camera.hpp"
#include "PrimitiveInstance.hpp"
#include <cstdint>

// Same as the OpenGL defined struct: DrawElementsIndirectCommand
struct DrawElementsIndirectCommand
{
    uint32_t count;         // Num elements (vertices)
    uint32_t instanceCount; // Number of instances to draw (a.k.a primcount)
    uint32_t firstIndex;    // Specifies a byte offset (cast to a pointer type) into the buffer bound to
                            // GL_ELEMENT_ARRAY_BUFFER to start reading indices from.
    uint32_t baseVertex;    // Specifies a constant that should be added to each element of indices​ when chosing
                            // elements from the enabled vertex arrays.
    uint32_t baseInstance;  // Specifies the base instance for use in fetching instanced vertex attributes.
};

struct ObjectData
{
    glm::mat4 model;
};

struct CameraCullingData
{
    glm::mat4 view;
    alignas(16) float fov;
    float aspect;
    float znear, zfar;

    glm::vec4 spherebounds;
    uint32_t drawCount;
};

struct InstanceIDData
{
    uint32_t id;
};

class PrimitiveIndirect : public PrimitiveInstance
{
  public:
    void Init(const std::shared_ptr<IMesh> &mesh, const std::shared_ptr<Shader> &shader,
              const std::shared_ptr<Shader> &shader_culling, const std::vector<glm::mat4> &instance_transforms);
    void Draw() override;

    void SetCamera(const std::shared_ptr<Camera> &camera)
    {
        camera_ = camera;
    }

  protected:
    virtual void CreateObjectDataBuffer();
    void CreateCameraCullingDataBuffer();
    virtual void CreateInstanceIDBuffer();
    virtual void CreateIndirectCommandBuffer();

    std::vector<ObjectData> object_data_;
    uint32_t object_data_buffer_ = -1;

    CameraCullingData camera_culling_data_;
    uint32_t camera_culling_data_buffer_ = -1;
    std::shared_ptr<Camera> camera_;

    std::vector<InstanceIDData> instance_id_data_;
    uint32_t instance_id_buffer_ = -1;

    DrawElementsIndirectCommand indirect_command_;
    uint32_t indirect_buffer_ = -1;

    std::shared_ptr<Shader> shader_culling_ = nullptr;
};