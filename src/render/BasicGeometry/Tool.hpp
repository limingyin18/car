#pragma once

#include <vector>

class Vertex;
class VertexNormalMap;

namespace graphics
{
// make sure every face vertex order, normal point to out side
void Order(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

void CalculateTangent(std::vector<VertexNormalMap> &vertices, const std::vector<uint32_t> &indices);
} // namespace graphics