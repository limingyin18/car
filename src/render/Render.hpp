#pragma once

#include <string>

#include "Primitive/Triangle.hpp"
#include "Shader/Shader.hpp"

class Render
{
public:
    Render();
    void Init(uint32_t width, uint32_t height);
    void Update();

    Triangle triangle;
    Shader shader_;
};