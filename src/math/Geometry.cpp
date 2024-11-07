#include "Geometry.hpp"

using namespace math;
using namespace Eigen;
using namespace std;

std::array<float, 3> math::Barycentric(Vector3f &p, Vector3f &a, Vector3f &b, Vector3f &c)
{
    std::array<float, 3> barycentric;

    Vector3f v0(b - a);
    Vector3f v1(c - a);
    Vector3f v2(p - a);
    float dot00 = v0.dot(v0);
    float dot01 = v0.dot(v1);
    float dot11 = v1.dot(v1);
    float dot20 = v2.dot(v0);
    float dot21 = v2.dot(v1);
    float denom = dot00 * dot11 - dot01 * dot01;
    barycentric[0] = (dot11 * dot20 - dot01 * dot21) / denom;
    barycentric[1] = (dot00 * dot21 - dot01 * dot20) / denom;
    barycentric[2] = 1.0f - barycentric[0] - barycentric[1];

    return barycentric;
}

Eigen::Vector3f math::NormalTriangle(Eigen::Vector3f const &a, Eigen::Vector3f const &b, Eigen::Vector3f const &c)
{
    return (b - a).cross(c - a).normalized();
}

Eigen::Vector3f math::NormalEdgeEdge(Eigen::Vector3f const &pA, Eigen::Vector3f const &pB, Eigen::Vector3f const &pC,
                                     Eigen::Vector3f const &pD)
{
    return (pB - pA).cross(pD - pC).normalized();
}

Eigen::Vector3f math::Tangent(const Eigen::Vector3f &N)
{
    Vector3f U1(N.cross(Vector3f{0.f, 0.f, 1.f}));
    Vector3f U2(N.cross(Vector3f{0.f, 1.f, 0.f}));
    if (U1.squaredNorm() > U2.squaredNorm())
        return U1.normalized();
    else
        return U2.normalized();
}