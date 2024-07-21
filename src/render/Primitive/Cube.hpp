#pragma once

#include "render/Primitive.hpp"

class Cube : public Primitive
{
public:
    void Init() override;

    glm::vec3 color_;
};