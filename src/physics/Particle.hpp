#pragma once

#include "glm/glm.hpp"

class Particle
{
  public:
    Particle(glm::vec3 pos = glm::vec3(0.f), glm::vec3 velocity = glm::vec3(0.f), float mass_inv = 1.f);
    virtual ~Particle() = default;

    void Update(float dt);
    void AddForce(glm::vec3 force);

    glm::vec3 pos_;
    glm::vec3 velocity_;
    float mass_inv_ = 1.f;

    glm::vec3 force_ = glm::vec3(0.f);
};