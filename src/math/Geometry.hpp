#pragma once

#include "Eigen/Core"
#include <Eigen/Eigen>

namespace math
{
/**
 * @brief 计算点p在三角形a, b, c上的重心坐标
 *
 * @param p 点p
 * @param a 三角形顶点a
 * @param b 三角形顶点b
 * @param c 三角形顶点c
 * @return std::array<float, 3> 重心坐标
 */
std::array<float, 3> Barycentric(Eigen::Vector3f &p, Eigen::Vector3f &a, Eigen::Vector3f &b, Eigen::Vector3f &c);

/**
 * @brief 计算三角形的法向量
 *
 * @param a 三角形顶点a
 * @param b 三角形顶点b
 * @param c 三角形顶点c
 * @return Eigen::Vector3f 法向量
 */
Eigen::Vector3f NormalTriangle(Eigen::Vector3f const &a, Eigen::Vector3f const &b, Eigen::Vector3f const &c);

/**
 * @brief 计算边边法向量
 *
 * @param pA 边A的一个端点
 * @param pB 边A的另一个端点
 * @param pC 边B的一个端点
 * @param pD 边B的另一个端点
 * @return Eigen::Vector3f 法向量
 */
Eigen::Vector3f NormalEdgeEdge(Eigen::Vector3f const &pA, Eigen::Vector3f const &pB, Eigen::Vector3f const &pC,
                               Eigen::Vector3f const &pD);

/**
 * @brief 计算切线 
 * 
 * @param N 法向量
 * @return Eigen::Vector3f 切线 
 */
Eigen::Vector3f Tangent(const Eigen::Vector3f &N);
} // namespace math