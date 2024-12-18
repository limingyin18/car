#include "CCD.hpp"
#include "Eigen/Core"
#include "Intersection.hpp"
#include "Response.hpp"

#include "math/Math.hpp"
#include "tools/Tool.hpp"
#include <algorithm>
#include <limits>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>

using namespace physics;
using namespace Eigen;
using namespace std;
using namespace math;

std::optional<float> physics::CCDPointTriangle(Eigen::Vector3f const &x0, Eigen::Vector3f const &x1,
                                               Eigen::Vector3f const &x2, Eigen::Vector3f const &x3,
                                               Eigen::Vector3f const &v0, Eigen::Vector3f const &v1,
                                               Eigen::Vector3f const &v2, Eigen::Vector3f const &v3)
{
    std::optional<float> t = FindCollisionTime(x0, x1, x2, x3, v0, v1, v2, v3);
    if (t.has_value())
    {
        Vector3f x0t = x0 + t.value() * v0;
        Vector3f x1t = x1 + t.value() * v1;
        Vector3f x2t = x2 + t.value() * v2;
        Vector3f x3t = x3 + t.value() * v3;
        if (IntersectPointTriangle(x0t, x1t, x2t, x3t))
        {
            return t;
        }
    }
    return std::nullopt;
}

std::optional<float> physics::FindCollisionTime(Eigen::Vector3f const &x0, Eigen::Vector3f const &x1,
                                                Eigen::Vector3f const &x2, Eigen::Vector3f const &x3,
                                                Eigen::Vector3f const &v0, Eigen::Vector3f const &v1,
                                                Eigen::Vector3f const &v2, Eigen::Vector3f const &v3)
{
    Vector3f x10f = x1 - x0;
    Vector3f x20f = x2 - x0;
    Vector3f x30f = x3 - x0;

    Vector3f v10f = v1 - v0;
    Vector3f v20f = v2 - v0;
    Vector3f v30f = v3 - v0;

    // float a = v30.dot(v10.cross(v20));
    // float b = x30.dot(v10.cross(v20)) + v30.dot(v10.cross(x20) + x10.cross(v20));
    // float c = x30.dot(v10.cross(x20) + x10.cross(v20)) + v30.dot(x10.cross(x20));
    // float d = x30.dot(x10.cross(x20));

    Vector3d x10 = Vector3d(x10f[0], x10f[1], x10f[2]);
    Vector3d x20 = Vector3d(x20f[0], x20f[1], x20f[2]);
    Vector3d x30 = Vector3d(x30f[0], x30f[1], x30f[2]);

    Vector3d v10 = Vector3d(v10f[0], v10f[1], v10f[2]);
    Vector3d v20 = Vector3d(v20f[0], v20f[1], v20f[2]);
    Vector3d v30 = Vector3d(v30f[0], v30f[1], v30f[2]);

    double a = v30.dot(v10.cross(v20));
    double b = x30.dot(v10.cross(v20)) + v30.dot(v10.cross(x20) + x10.cross(v20));
    double c = x30.dot(v10.cross(x20) + x10.cross(v20)) + v30.dot(x10.cross(x20));
    double d = x30.dot(x10.cross(x20));



    // spdlog::info("a: {}, b: {}, c: {}, d: {}", a, b, c, d);

    // Vector3f x21 = x1 - x0;
    // Vector3f x31 = x2 - x0;
    // Vector3f x41 = x3 - x0;
    // Vector3f v21 = v1 - v0;
    // Vector3f v31 = v2 - v0;
    // Vector3f v41 = v3 - v0;

    // float a = (-(v21.z() * v31.y() * v41.x()) + v21.y() * v31.z() * v41.x() + v21.z() * v31.x() * v41.y() -
    //            v21.x() * v31.z() * v41.y() - v21.y() * v31.x() * v41.z() + v21.x() * v31.y() * v41.z());

    // float b = (-(v31.x() * v41.z() * x21.y()) + v31.x() * v41.y() * x21.z() + v21.z() * v41.y() * x31.x() -
    //            v21.y() * v41.z() * x31.x() - v21.z() * v41.x() * x31.y() + v21.x() * v41.z() * x31.y() +
    //            v21.y() * v41.x() * x31.z() - v21.x() * v41.y() * x31.z() + v21.z() * v31.x() * x41.y() +
    //            v31.z() * (-(v41.y() * x21.x()) + v41.x() * x21.y() + v21.y() * x41.x() - v21.x() * x41.y()) -
    //            v21.y() * v31.x() * x41.z() +
    //            v31.y() * (v41.z() * x21.x() - v41.x() * x21.z() - v21.z() * x41.x() + v21.x() * x41.z()));

    // float c = (-(v41.z() * x21.y() * x31.x()) + v41.y() * x21.z() * x31.x() + v41.z() * x21.x() * x31.y() -
    //            v41.x() * x21.z() * x31.y() - v41.y() * x21.x() * x31.z() + v41.x() * x21.y() * x31.z() +
    //            v31.z() * x21.y() * x41.x() - v31.y() * x21.z() * x41.x() - v21.z() * x31.y() * x41.x() +
    //            v21.y() * x31.z() * x41.x() - v31.z() * x21.x() * x41.y() + v31.x() * x21.z() * x41.y() +
    //            v21.z() * x31.x() * x41.y() - v21.x() * x31.z() * x41.y() + v31.y() * x21.x() * x41.z() -
    //            v31.x() * x21.y() * x41.z() - v21.y() * x31.x() * x41.z() + v21.x() * x31.y() * x41.z());

    // float d = -x21.z() * x31.y() * x41.x() + x21.y() * x31.z() * x41.x() + x21.z() * x31.x() * x41.y() -
    //           x21.x() * x31.z() * x41.y() - x21.y() * x31.x() * x41.z() + x21.x() * x31.y() * x41.z();
    // spdlog::info("a: {}, b: {}, c: {}, d: {}", a, b, c, d);
    if (a == 0 && b == 0 && c == 0 && d == 0)
    {
        // float len_x31 = x31.norm();
        // float len_v31 = v31.norm();

        float len_x31 = x20.norm();
        float len_v31 = v20.norm();
        float t = len_x31 / len_v31;
        if (t < 0 || t > 1)
        {
            return std::nullopt;
        }
        return len_x31 / len_v31;
    }

    float roots[3] = {0};
    int nRoots = cubic_solver(a, b, c, d, roots); // 0 <= t <= 1

    std::vector<float> validRoots;
    for (int i = 0; i < nRoots; i++)
    {
        if (roots[i] >= 0 && roots[i] <= 1)
        {
            validRoots.push_back(roots[i]);
        }
    }

    if (validRoots.size() == 0)
    {
        return std::nullopt;
    }

    return *std::ranges::min_element(validRoots);
}

std::optional<std::pair<float, std::array<float, 2>>> physics::CCDEdgeEdge(
    Eigen::Vector3f const &x0, Eigen::Vector3f const &x1, Eigen::Vector3f const &x2, Eigen::Vector3f const &x3,
    Eigen::Vector3f const &v0, Eigen::Vector3f const &v1, Eigen::Vector3f const &v2, Eigen::Vector3f const &v3)
{
    std::optional<float> t = FindCollisionTime(x0, x1, x2, x3, v0, v1, v2, v3);
    if (t.has_value())
    {
        Vector3f x0t = x0 + t.value() * v0;
        Vector3f x1t = x1 + t.value() * v1;
        Vector3f x2t = x2 + t.value() * v2;
        Vector3f x3t = x3 + t.value() * v3;
        auto st = IntersectEdgeEdge(x0t, x1t, x2t, x3t);
        if (st.has_value())
        {
            return std::pair{t.value(), st.value()};
        }
    }
    return std::nullopt;
}

std::optional<float> physics::CCDTriangleTriangle(Particle &particle0, Particle &particle1, Particle &particle2,
                                                  Particle &particle3, Particle &particle4, Particle &particle5,
                                                  float dt)
{
    float result = std::numeric_limits<float>::max();

    auto PointTriangle = [dt, &result](Particle &p, Particle &p1, Particle &p2, Particle &p3) {
        Vector3f x0 = p.pos_;
        Vector3f x1 = p1.pos_;
        Vector3f x2 = p2.pos_;
        Vector3f x3 = p3.pos_;

        Vector3f v0 = p.velocity_ * dt;
        Vector3f v1 = p1.velocity_ * dt;
        Vector3f v2 = p2.velocity_ * dt;
        Vector3f v3 = p3.velocity_ * dt;

        auto t = CCDPointTriangle(x0, x1, x2, x3, v0, v1, v2, v3);
        if (t.has_value())
        {
            ResponsePointTriangle(p, p1, p2, p3, t.value() * dt);
            result = std::min(t.value(), result);
        }
    };

    PointTriangle(particle0, particle3, particle4, particle5);
    PointTriangle(particle1, particle3, particle4, particle5);
    PointTriangle(particle2, particle3, particle4, particle5);
    PointTriangle(particle3, particle0, particle1, particle2);
    PointTriangle(particle4, particle0, particle1, particle2);
    PointTriangle(particle5, particle0, particle1, particle2);

    auto Edge2Edge = [dt, &result](Particle &p0, Particle &p1, Particle &p2, Particle &p3) {
        Vector3f x0 = p0.pos_;
        Vector3f x1 = p1.pos_;
        Vector3f x2 = p2.pos_;
        Vector3f x3 = p3.pos_;

        Vector3f v0 = p0.velocity_ * dt;
        Vector3f v1 = p1.velocity_ * dt;
        Vector3f v2 = p2.velocity_ * dt;
        Vector3f v3 = p3.velocity_ * dt;

        auto t = CCDEdgeEdge(x0, x1, x2, x3, v0, v1, v2, v3);
        if (t.has_value())
        {
            ResponseEdgeEdge(p0, p1, p2, p3, t.value().first * dt, t.value().second[0], t.value().second[1]);
            result = std::min(t.value().first, result);
        }
    };

    Edge2Edge(particle0, particle1, particle3, particle4);
    Edge2Edge(particle0, particle1, particle3, particle5);
    Edge2Edge(particle0, particle1, particle4, particle5);
    Edge2Edge(particle0, particle2, particle3, particle4);
    Edge2Edge(particle0, particle2, particle3, particle5);
    Edge2Edge(particle0, particle2, particle4, particle5);
    Edge2Edge(particle1, particle2, particle3, particle4);
    Edge2Edge(particle1, particle2, particle3, particle5);
    Edge2Edge(particle1, particle2, particle4, particle5);

    if (result == std::numeric_limits<float>::max())
    {
        return std::nullopt;
    }
    return result * dt;
}

std::optional<float> physics::CCDTriangleTriangleCheck(Particle &particle0, Particle &particle1, Particle &particle2,
                                                       Particle &particle3, Particle &particle4, Particle &particle5,
                                                       float dt)
{
    float result = std::numeric_limits<float>::max();

    auto PointTriangle = [dt, &result](Particle &p, Particle &p1, Particle &p2, Particle &p3) {
        Vector3f x0 = p.pos_;
        Vector3f x1 = p1.pos_;
        Vector3f x2 = p2.pos_;
        Vector3f x3 = p3.pos_;

        Vector3f v0 = p.velocity_ * dt;
        Vector3f v1 = p1.velocity_ * dt;
        Vector3f v2 = p2.velocity_ * dt;
        Vector3f v3 = p3.velocity_ * dt;

        auto t = CCDPointTriangle(x0, x1, x2, x3, v0, v1, v2, v3);
        if (t.has_value())
        {
            result = std::min(t.value(), result);
        }
    };

    PointTriangle(particle0, particle3, particle4, particle5);
    PointTriangle(particle1, particle3, particle4, particle5);
    PointTriangle(particle2, particle3, particle4, particle5);
    PointTriangle(particle3, particle0, particle1, particle2);
    PointTriangle(particle4, particle0, particle1, particle2);
    PointTriangle(particle5, particle0, particle1, particle2);

    auto Edge2Edge = [dt, &result](Particle &p0, Particle &p1, Particle &p2, Particle &p3) {
        Vector3f x0 = p0.pos_;
        Vector3f x1 = p1.pos_;
        Vector3f x2 = p2.pos_;
        Vector3f x3 = p3.pos_;

        Vector3f v0 = p0.velocity_ * dt;
        Vector3f v1 = p1.velocity_ * dt;
        Vector3f v2 = p2.velocity_ * dt;
        Vector3f v3 = p3.velocity_ * dt;

        auto t = CCDEdgeEdge(x0, x1, x2, x3, v0, v1, v2, v3);
        if (t.has_value())
        {
            result = std::min(t.value().first, result);
        }
    };

    Edge2Edge(particle0, particle1, particle3, particle4);
    Edge2Edge(particle0, particle1, particle3, particle5);
    Edge2Edge(particle0, particle1, particle4, particle5);
    Edge2Edge(particle0, particle2, particle3, particle4);
    Edge2Edge(particle0, particle2, particle3, particle5);
    Edge2Edge(particle0, particle2, particle4, particle5);
    Edge2Edge(particle1, particle2, particle3, particle4);
    Edge2Edge(particle1, particle2, particle3, particle5);
    Edge2Edge(particle1, particle2, particle4, particle5);

    if (result == std::numeric_limits<float>::max())
    {
        return std::nullopt;
    }
    return result * dt;
}