#include "Render.hpp"

#include "glad/glad.h"

Render::Render() : shader_("shaders/basic.vert", "shaders/basic.frag")
{
}

void Render::Init(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    triangle.Init();
    triangle.SetShader(shader_);
}

void Render::Update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    triangle.Draw();
}