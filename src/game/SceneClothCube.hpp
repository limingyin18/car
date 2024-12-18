#pragma once

#include "Scene.hpp"

#include <Eigen/Eigen>

class Cloth;
class Mesh;
class Edge;

class SceneClothCube : public Scene
{
  public:
    void Init() override;
    void Update() override;
    void UpdatePhysics() override;

  private:
    void InitCloth();
    void InitCube();
    void MeshAdjacentList(Mesh &mesh);
    std::pair<float, Eigen::Vector3f> sdf(const Eigen::Vector3f &pos);

    void ApplyReplusionForce();
    void CollideClothCube();
    void UpdateClothPosition();

    std::shared_ptr<Cloth> cloth_;
    std::shared_ptr<Actor> cube_;

    std::vector<std::pair<uint32_t, uint32_t>> adjacency_list_;
    std::map<Edge, std::pair<uint32_t, uint32_t>> edge_to_triangle_;
};