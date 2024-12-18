#pragma once

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
std::array<float, 3> Barycentric(const Eigen::Vector3f &p, const Eigen::Vector3f &a, const Eigen::Vector3f &b,
                                 const Eigen::Vector3f &c);

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

/**
 * @brief 计算线段与线段的最近点
 *
 * @param p1 线段1的一个端点
 * @param q1 线段1的另一个端点
 * @param p2 线段2的一个端点
 * @param q2 线段2的另一个端点
 * @param s 线段1上的参数
 * @param t 线段2上的参数
 * @param c1 最近点1
 * @param c2 最近点2
 * @return float 最近点之间的距离
 */
float CloestPtSegmentSegment(Eigen::Vector3f const &p1, Eigen::Vector3f const &q1, Eigen::Vector3f const &p2,
                             Eigen::Vector3f const &q2, float &s, float &t, Eigen::Vector3f &c1, Eigen::Vector3f &c2);

/**
 * @brief 计算点到平面的最近点
 *
 * @param q 点
 * @param p 平面上的一点
 * @param n 平面的法向量
 * @return Eigen::Vector3f 最近点
 */
Eigen::Vector3f ClosestPtPointPlane(Eigen::Vector3f const &q, Eigen::Vector3f const &p, Eigen::Vector3f const &n);
} // namespace math