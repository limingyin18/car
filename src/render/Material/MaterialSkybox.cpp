#include "MaterialSkybox.hpp"

#include "glad/glad.h"

void MaterialSkybox::Use(const std::shared_ptr<Shader> shader)
{
    Material::Use(shader);

    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
}