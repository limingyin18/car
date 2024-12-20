#include "Game.hpp"

#include "Actor/ActorSkeletalIndirectLod.hpp"
#include "BasicActor/Cloth.hpp"
#include "BasicActor/Ocean.hpp"
#include "BasicActor/Skybox.hpp"
#include "render/BasicGeometry/Plane.hpp"
#include "render/Material/Material.hpp"
#include "render/Mesh/Mesh.hpp"
#include "render/Mesh/Vertex.hpp"
#include "render/Render.hpp"


#include <cstddef>
#include <memory>

using namespace std;

// void Game::InitBasicGeometry()
// {
//     auto &render = Render::GetInstance();
//     auto &shaders_map = render.GetShadersMap();
//     auto &meshes_map = render.GetMeshesMap();

//     if (true)
//     {
//         for (size_t i = 0; i < 10; i++)
//         {
//             for (size_t j = 0; j < 10; j++)
//             {
//                 float x = static_cast<float>(i);
//                 float y = static_cast<float>(j);

//                 auto sphere = std::make_shared<Actor>();
//                 sphere->AddMesh(meshes_map["sphere"]);
//                 sphere->SetShader(shaders_map["pbr_uniform"]);
//                 auto material = std::make_shared<Material>();
//                 material->SetVec3("albedo", glm::vec3{1.0f, 0.0f, 0.0f});
//                 material->SetFloat("metallic", x / 10.0f);
//                 material->SetFloat("roughness", y / 10.0f);
//                 material->SetFloat("ao", 1.0f);

//                 Texture texture;
//                 texture.id = render.GetConvolutionCubemap();
//                 material->SetTexture(4, texture);
//                 texture.id = render.GetPrefilterCubemap();
//                 material->SetTexture(5, texture);
//                 texture.id = render.GetBRDF();
//                 material->SetTexture(6, texture);

//                 sphere->SetMaterial(material);
//                 glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{x, y, 0.0f});
//                 sphere->SetTransform(transform);
//                 actors_.push_back(sphere);
//             }
//         }
//     }
// }

// void Game::InitShields()
// {
//     auto &render = Render::GetInstance();
//     auto &shaders_map = render.GetShadersMap();
//     auto &meshes_map = render.GetMeshesMap();

//     uint32_t instance_count = 100;
//     std::vector<glm::mat4> instance_transforms(instance_count * instance_count);
//     for (uint32_t i = 0; i < instance_count; i++)
//     {
//         for (uint32_t j = 0; j < instance_count; j++)
//         {
//             uint32_t index = j + i * instance_count;
//             auto &transform = instance_transforms[index];
//             transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.f, 1.f, 1.f));
//             transform = glm::rotate(transform, glm::radians(0.f), glm::vec3(1, 0, 0));
//             transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f, 0.0f, j * 1.0f)) * transform;
//         }
//     }

//     shield_ = std::make_shared<ActorSkeletalIndirectLod>();
//     shield_->SetModelPath("assets/Shield/sword.gltf");
//     shield_->SetShader(shaders_map["skeletal_phong_indirect_lod"]);
//     shield_->SetShaderCulling(shaders_map["culling_lod"]);
//     shield_->SetInstanceTransforms(instance_transforms);
//     shield_->AddAnimation("assets/Shield/idle.gltf");
//     shield_->AddAnimation("assets/Shield/walk.gltf");
//     shield_->AddAnimation("assets/Shield/slash.gltf");
//     shield_->AddAnimation("assets/Shield/death.gltf");

//     actors_.push_back(shield_);
// }

// void Game::InitArrows()
// {
//     auto &render = Render::GetInstance();
//     auto &shaders_map = render.GetShadersMap();
//     auto &meshes_map = render.GetMeshesMap();

//     uint32_t instance_count = 50;
//     std::vector<glm::mat4> instance_transforms(instance_count * instance_count);
//     for (uint32_t i = 0; i < instance_count; i++)
//     {
//         for (uint32_t j = 0; j < instance_count; j++)
//         {
//             uint32_t index = j + i * instance_count;
//             auto &transform = instance_transforms[index];
//             transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.f, 1.f, 1.f));
//             transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0, 1, 0));
//             transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f, 0.0f, 100 + 10 + j * 1.0f)) * transform;
//         }
//     }

//     arrow_ = std::make_shared<ActorSkeletalIndirectLod>();
//     arrow_->SetModelPath("assets/Arrow/arrow.gltf");
//     arrow_->SetShader(shaders_map["skeletal_phong_indirect_lod"]);
//     arrow_->SetShaderCulling(shaders_map["culling_lod"]);
//     arrow_->SetInstanceTransforms(instance_transforms);
//     arrow_->AddAnimation("assets/arrow/idle.gltf");
//     arrow_->AddAnimation("assets/arrow/walk.gltf");
//     arrow_->AddAnimation("assets/arrow/attack.gltf");
//     arrow_->AddAnimation("assets/arrow/death.gltf");

//     actors_.push_back(arrow_);
// }

// void Game::InitHouse()
// {
//     auto &render = Render::GetInstance();
//     auto &shaders_map = render.GetShadersMap();
//     auto &meshes_map = render.GetMeshesMap();

//     house_ = std::make_shared<Actor>();
//     house_->SetModelPath("assets/Sponza/glTF/Sponza.gltf");
//     // house_->SetModelPath("assets/ABeautifulGame/glTF/ABeautifulGame.gltf");
//     house_->SetShader(shaders_map["pbr"]);

//     Texture texture;
//     texture.id = render.GetConvolutionCubemap();
//     auto material = std::make_shared<Material>();
//     material->SetTexture(3, texture);
//     texture.id = render.GetPrefilterCubemap();
//     material->SetTexture(4, texture);
//     texture.id = render.GetBRDF();
//     material->SetTexture(5, texture);
//     house_->SetMaterial(material);
//     // house_->SetTransform(glm::scale(glm::mat4(1.0f), glm::vec3(10.f)));

//     actors_.push_back(house_);
// }

// void Game::InitCubes()
// {
//     auto &render = Render::GetInstance();
//     auto &shaders_map = render.GetShadersMap();
//     auto &meshes_map = render.GetMeshesMap();

//     uint32_t instance_count = 10;
//     std::vector<glm::mat4> instance_transforms(instance_count * instance_count);
//     for (uint32_t i = 0; i < instance_count; i++)
//     {
//         for (uint32_t j = 0; j < instance_count; j++)
//         {
//             uint32_t index = j + i * instance_count;
//             auto &transform = instance_transforms[index];
//             transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
//             transform = glm::rotate(transform, glm::radians(0.f), glm::vec3(0, 1, 0));
//             transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f, 0.0f, j * 1.0f)) * transform;
//         }
//     }

//     cube_ = std::make_shared<ActorIndirect>();
//     cube_->AddMesh(meshes_map["cube"]);
//     cube_->SetShader(shaders_map["phong_indirect"]);
//     cube_->SetShaderCulling(shaders_map["culling"]);
//     cube_->SetInstanceTransforms(instance_transforms);
//     actors_.push_back(cube_);
// }

// void Game::InitPlane()
// {
//     auto &render = Render::GetInstance();
//     auto &shaders_map = render.GetShadersMap();
//     auto &meshes_map = render.GetMeshesMap();

//     plane_ = std::make_shared<Actor>();
//     plane_->AddMesh(meshes_map["plane"]);
//     plane_->SetShader(shaders_map["phong"]);

//     auto material = std::make_shared<Material>();
//     Texture texture;
//     texture.id = render.GetTexture("default_yellow1");
//     material->SetTexture(0, texture);
//     material->SetFloat("ambientStrength", 0.0f);
//     plane_->SetMaterial(material);

//     plane_->SetTransform(glm::scale(glm::mat4(1.0f), glm::vec3(1000.f)));

//     actors_.push_back(plane_);
// }

// void Game::InitOcean()
// {
//     auto &render = Render::GetInstance();
//     auto &shaders_map = render.GetShadersMap();
//     auto &meshes_map = render.GetMeshesMap();

//     auto plane = make_shared<Plane>();
//     plane->Init(32, 100.f);

//     ocean_ = std::make_shared<Ocean>();
//     ocean_->AddMesh(plane);
//     ocean_->SetShader(shaders_map["ocean"]);
//     ocean_->SetShaderAmpl(shaders_map["ocean_ampl"]);
//     ocean_->SetShaderIfft2(shaders_map["ifft2"]);
//     ocean_->SetShaderSign(shaders_map["ocean_sign"]);

//     actors_.push_back(ocean_);
// }

// void Game::InitCloth()
// {
//     auto &render = Render::GetInstance();
//     auto &shaders_map = render.GetShadersMap();
//     auto &meshes_map = render.GetMeshesMap();

//     auto plane = make_shared<Plane>();
//     plane->SetDynamic();

//     plane->Init(4, 10.f);
//     cloth_ = make_shared<Cloth>();
//     cloth_->AddMesh(plane);
//     cloth_->SetShader(shaders_map["phong"]);

//     auto material = std::make_shared<Material>();
//     Texture texture;
//     texture.id = render.GetTexture("default_red");
//     material->SetTexture(0, texture);
//     material->SetFloat("ambientStrength", 0.1f);
//     material->SetFloat("specularStrength", 0.5f);
//     material->SetFloat("specularPow", 255.f);

//     cloth_->SetMaterial(material);

//     cloth_->GeneratePoints();
//     cloth_->GenerateSegments();
//     cloth_->InitPhysics();

//     actors_.push_back(cloth_);
// }