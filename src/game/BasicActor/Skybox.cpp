#include "Skybox.hpp"
#include "game/Actor/Actor.hpp"
#include "render/Material/MaterialSkybox.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"
#include <memory>

using namespace std;

void Skybox::Init()
{
    auto &render = Render::GetInstance();
    auto &shaders_map = render.GetShadersMap();
    auto &meshes_map = render.GetMeshesMap();
    AddMesh(meshes_map["cube"]);
    SetShader(shaders_map["skybox"]);
    material_ = std::make_shared<MaterialSkybox>();
    Texture texture;
    // texture.id = render.GetTextureHDR("assets/textures/hdr/newport_loft.hdr");
    texture.id = render.GetEnvCubemap();
    // texture.id = render.GetPrefilterCubemap();
    material_->SetTexture(0, texture);

    Camera &camera = *render.GetCamera();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetView()));
    material_->SetMat4("projection", camera.GetProj());
    material_->SetMat4("view", view);

    Actor::Init();
}

void Skybox::Update()
{
    // material_->SetMat4("view", glm::mat4(glm::mat3(Render::GetInstance().GetCamera()->GetView())));
    // primitives_[0]->SetMaterial(material_);
    Actor::Update();
}