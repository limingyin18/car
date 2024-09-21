#include "Render.hpp"

#include "render/BasicGeometry/Cube.hpp"
#include "render/BasicGeometry/Plane.hpp"
#include "render/BasicGeometry/Sphere.hpp"
#include "render/Mesh/Vertex.hpp"


using namespace std;

void Render::CreateBasicGeometry()
{
    CreateCube();
    CreateSphere();
    CreatePlane();
}

void Render::CreateCube()
{
    auto cube = make_shared<Cube>();
    cube->Init();
    Texture texture;
    texture.id = textures_map_["uvgrid"];
    texture.type = "texture_diffuse";
    texture.path = "";
    std::vector<Texture> textures;
    textures.push_back(texture);
    cube->SetTextures(textures);

    meshes_map_["cube"] = cube;
}

void Render::CreateSphere()
{
    auto sphere = make_shared<Sphere>();
    sphere->Init(360, 180, 0.5f);
    Texture texture;
    texture.id = textures_map_["uvgrid"];
    texture.type = "texture_diffuse";
    texture.path = "";
    std::vector<Texture> textures;
    textures.push_back(texture);
    sphere->SetTextures(textures);

    meshes_map_["sphere"] = sphere;
}

void Render::CreatePlane()
{
    auto plane = make_shared<Plane>();
    plane->Init(2);
    Texture texture;
    texture.id = textures_map_["default_white"];
    texture.type = "texture_diffuse";
    texture.path = "";
    std::vector<Texture> textures;
    textures.push_back(texture);
    plane->SetTextures(textures);

    meshes_map_["plane"] = plane;
}