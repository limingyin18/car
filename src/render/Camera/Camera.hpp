#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <spdlog/spdlog.h>

class Camera
{
  public:
    Camera() = default;
    ~Camera()
    {
        spdlog::debug("camera destruct");
    };

    glm::mat4 GetView() const
    {
        return view_;
    };
    glm::mat4 GetProj() const
    {
        return proj_;
    };

    void LookAt()
    {
        view_ = glm::lookAt(position_, position_ + front_, up_);
    };
    void Proj()
    {
        proj_ = glm::perspective(glm::radians(fov_), width_ / height_, near_, far_);
    };

    glm::vec3 GetPosition()
    {
        return position_;
    };

    void SetPosition(const glm::vec3 &position)
    {
        position_ = position;
    };
    void SetUp(const glm::vec3 &up)
    {
        up_ = glm::normalize(up);
        right_ = glm::cross(front_, up_);
    };
    void SetFront(const glm::vec3 &front)
    {
        front_ = glm::normalize(front);
        // up_ = glm::vec3(0.f, 1.0f, 0.f);
        right_ = glm::cross(front_, up_);
        // up_ = glm::cross(right_, front_);
    };
    void SetFov(float fov)
    {
        fov_ = fov;
    };
    void SetWidth(float width)
    {
        width_ = width;
    };
    void SetHeight(float height)
    {
        height_ = height;
    };
    void SetNear(float near)
    {
        near_ = near;
    };
    void SetFar(float far)
    {
        far_ = far;
    };
    float GetFov() const
    {
        return fov_;
    };
    float GetWidth() const
    {
        return width_;
    };
    float GetHeight() const
    {
        return height_;
    };
    float GetNear() const
    {
        return near_;
    };
    float GetFar() const
    {
        return far_;
    };
    float GetAspect() const
    {
        return width_ / height_;
    };

    void MoveForward(float movement)
    {
        position_ += front_ * movement;
    };
    void MoveRight(float movement)
    {
        position_ += right_ * movement;
    };
    void MoveUp(float movement)
    {
        position_ += up_ * movement;
    };
    void RotateYaw(float rotation)
    {
        front_ = glm::rotate(front_, rotation, up_);
        right_ = glm::rotate(right_, rotation, up_);
    };
    void RotatePitch(float rotation)
    {
        front_ = glm::rotate(front_, rotation, right_);
        // up_ = glm::rotate(up_, rotation, right_);
    };

  private:
    glm::vec3 position_ = glm::vec3(0.f, 0.f, 1.f);
    glm::vec3 front_ = glm::vec3(0.f, 0.f, -1.f);
    glm::vec3 up_ = glm::vec3(0.f, 1.f, 0.f);
    glm::vec3 right_ = glm::vec3(1.f, 0.f, 0.f);

    float fov_ = 45.f;
    float width_ = 1080, height_ = 960;
    float near_ = 0.1f, far_ = 1000.f;

    glm::mat4 view_{1.0f};
    glm::mat4 proj_{1.0f};
};