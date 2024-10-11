#pragma once

#include "game/Actor/Actor.hpp"
#include <cstdint>
#include <iostream>

class Cube;
class Sphere;

class Edge
{
  public:
    uint32_t vertex_id_start_;
    uint32_t vertex_id_end_;

    friend std::ostream &operator<<(std::ostream &os, const Edge &edge);
};

class Triple
{
  public:
    uint32_t vertex_id_start_;
    uint32_t vertex_id_end_;
    uint32_t triangle_id_;

    Triple(uint32_t vertex_id_start, uint32_t vertex_id_end, uint32_t triangle_id);
    virtual ~Triple() = default;

    bool operator<(const Triple &rhs) const;
    friend std::ostream &operator<<(std::ostream &os, const Triple &triple);
};

class Cloth : public Actor
{
  private:
    std::vector<std::shared_ptr<Actor>> segments_;
    std::shared_ptr<Actor> points_;

    std::vector<Edge> edges_;

  public:
    explicit Cloth() = default;
    virtual ~Cloth() = default;
    Cloth(const Cloth &rhs) = delete;
    Cloth(Cloth &&rhs) = delete;
    Cloth &operator=(const Cloth &rhs) = delete;
    Cloth &operator=(Cloth &&rhs) = delete;

    void Init() override;
    void Update() override;

    void GeneratePoints();
    void GenerateSegments();
    void InitPhysics();
};