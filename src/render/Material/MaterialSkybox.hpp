#pragma once

#include "render/Material/Material.hpp"

class MaterialSkybox : public Material
{
  public:
    void Use(const std::shared_ptr<Shader> shader) override;
};