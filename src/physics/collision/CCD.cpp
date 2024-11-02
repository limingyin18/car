#include "CCD.hpp"
#include "Intersection.hpp"

using namespace physics;
using namespace Eigen;
using namespace std;

std::optional<float> physics::CCDPointTriangle(Eigen::Vector3f const &x0, Eigen::Vector3f const &x1,
                                               Eigen::Vector3f const &x2, Eigen::Vector3f const &x3,
                                               Eigen::Vector3f const &v0, Eigen::Vector3f const &v1,
                                               Eigen::Vector3f const &v2, Eigen::Vector3f const &v3)
{
    Vector3f x10 = x1 - x0;
    Vector3f x20 = x2 - x0;
    Vector3f x30 = x3 - x0;

    Vector3f v10 = v1 - v0;
    Vector3f v20 = v2 - v0;
    Vector3f v30 = v3 - v0;

    float a = v30.dot(v10.cross(v20));
    float b = x30.dot(v10.cross(v20)) + v30.dot(v10.cross(x20) + x10.cross(v20));
    float c = x30.dot(v10.cross(x20) + x10.cross(v20)) + v30.dot(v10.cross(x20));
    float d = x30.dot(x10.cross(x20));

    float roots[3] = {0};
    int nRoots = cubic_solver(a, b, c, d, roots); // 0 <= t <= 1
    if (nRoots == 0)
        return std::nullopt;
    switch (nRoots)
    {
    case 0:
        return std::nullopt;
    case 1:
        return roots[0];
        break;
    case 2:
        return std::min(roots[0], roots[1]);
        break;
    case 3:
        return std::min(std::min(roots[0], roots[1]), roots[2]);
        break;
    default:
        return std::nullopt;
    }
}