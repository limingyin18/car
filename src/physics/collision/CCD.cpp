#include "CCD.hpp"
#include "Eigen/Core"
#include "Intersection.hpp"
#include "Response.hpp"

#include <limits>
#include <optional>
#include <spdlog/spdlog.h>

using namespace physics;
using namespace Eigen;
using namespace std;

std::optional<float> physics::CCDPointTriangle(Eigen::Vector3f const &x0, Eigen::Vector3f const &x1,
                                               Eigen::Vector3f const &x2, Eigen::Vector3f const &x3,
                                               Eigen::Vector3f const &v0, Eigen::Vector3f const &v1,
                                               Eigen::Vector3f const &v2, Eigen::Vector3f const &v3)
{
    std::optional<float> t = FindCollisionTime(x0, x1, x2, x3, v0, v1, v2, v3);
    spdlog::debug("collision time t");
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
    spdlog::debug("begin find collision time");
    Vector3f x10 = x1 - x0;
    Vector3f x20 = x2 - x0;
    Vector3f x30 = x3 - x0;

    Vector3f v10 = v1 - v0;
    Vector3f v20 = v2 - v0;
    Vector3f v30 = v3 - v0;

    float a = v30.dot(v10.cross(v20));
    float b = x30.dot(v10.cross(v20)) + v30.dot(v10.cross(x20) + x10.cross(v20));
    float c = x30.dot(v10.cross(x20) + x10.cross(v20)) + v30.dot(x10.cross(x20));
    float d = x30.dot(x10.cross(x20));
    // spdlog::info("a: {}, b: {}, c: {}, d: {}", a, b, c, d);

    /*
        Vector3f x21 = x1 - x0;
        Vector3f x31 = x2 - x0;
        Vector3f x41 = x3 - x0;
        Vector3f v21 = v1 - v0;
        Vector3f v31 = v2 - v0;
        Vector3f v41 = v3 - v0;

        float a = (-(v21.z() * v31.y() * v41.x()) + v21.y() * v31.z() * v41.x() + v21.z() * v31.x() * v41.y() -
                   v21.x() * v31.z() * v41.y() - v21.y() * v31.x() * v41.z() + v21.x() * v31.y() * v41.z());

        float b = (-(v31.x() * v41.z() * x21.y()) + v31.x() * v41.y() * x21.z() + v21.z() * v41.y() * x31.x() -
                   v21.y() * v41.z() * x31.x() - v21.z() * v41.x() * x31.y() + v21.x() * v41.z() * x31.y() +
                   v21.y() * v41.x() * x31.z() - v21.x() * v41.y() * x31.z() + v21.z() * v31.x() * x41.y() +
                   v31.z() * (-(v41.y() * x21.x()) + v41.x() * x21.y() + v21.y() * x41.x() - v21.x() * x41.y()) -
                   v21.y() * v31.x() * x41.z() +
                   v31.y() * (v41.z() * x21.x() - v41.x() * x21.z() - v21.z() * x41.x() + v21.x() * x41.z()));

        float c = (-(v41.z() * x21.y() * x31.x()) + v41.y() * x21.z() * x31.x() + v41.z() * x21.x() * x31.y() -
                   v41.x() * x21.z() * x31.y() - v41.y() * x21.x() * x31.z() + v41.x() * x21.y() * x31.z() +
                   v31.z() * x21.y() * x41.x() - v31.y() * x21.z() * x41.x() - v21.z() * x31.y() * x41.x() +
                   v21.y() * x31.z() * x41.x() - v31.z() * x21.x() * x41.y() + v31.x() * x21.z() * x41.y() +
                   v21.z() * x31.x() * x41.y() - v21.x() * x31.z() * x41.y() + v31.y() * x21.x() * x41.z() -
                   v31.x() * x21.y() * x41.z() - v21.y() * x31.x() * x41.z() + v21.x() * x31.y() * x41.z());

        float d = -x21.z() * x31.y() * x41.x() + x21.y() * x31.z() * x41.x() + x21.z() * x31.x() * x41.y() -
                  x21.x() * x31.z() * x41.y() - x21.y() * x31.x() * x41.z() + x21.x() * x31.y() * x41.z();
        spdlog::info("a: {}, b: {}, c: {}, d: {}", a, b, c, d);
                  */

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

    Vector3f p1 = particle0.pos_;
    Vector3f q1 = particle1.pos_;
    Vector3f r1 = particle2.pos_;
    Vector3f p2 = particle3.pos_;
    Vector3f q2 = particle4.pos_;
    Vector3f r2 = particle5.pos_;
    Vector3f v1 = particle0.velocity_ * dt;
    Vector3f v2 = particle1.velocity_ * dt;
    Vector3f v3 = particle2.velocity_ * dt;
    Vector3f v4 = particle3.velocity_ * dt;
    Vector3f v5 = particle4.velocity_ * dt;
    Vector3f v6 = particle5.velocity_ * dt;

    std::optional<float> t0 = CCDPointTriangle(p1, p2, q2, r2, v1, v4, v5, v6);
    if (t0.has_value())
    {
        ResponsePointTriangle(particle0, particle3, particle4, particle5, t0.value() * dt);
        result = std::min(t0.value(), result);
    }
    spdlog::debug("t0");

    std::optional<float> t1 = CCDPointTriangle(q1, p2, q2, r2, v2, v4, v5, v6);
    if (t1.has_value())
    {
        ResponsePointTriangle(particle1, particle3, particle4, particle5, t1.value() * dt);
        result = std::min(t1.value(), result);
    }
    spdlog::debug("t1");

    std::optional<float> t2 = CCDPointTriangle(r1, p2, q2, r2, v3, v4, v5, v6);
    if (t2.has_value())
    {
        ResponsePointTriangle(particle2, particle3, particle4, particle5, t2.value() * dt);
        result = std::min(t2.value(), result);
    }
    std::optional<float> t3 = CCDPointTriangle(p2, p1, q1, r1, v4, v1, v2, v3);
    if (t3.has_value())
    {
        ResponsePointTriangle(particle3, particle3, particle4, particle5, t2.value() * dt);
        result = std::min(t3.value(), result);
    }
    std::optional<float> t4 = CCDPointTriangle(q2, p1, q1, r1, v5, v1, v2, v3);
    if (t4.has_value())
    {
        ResponsePointTriangle(particle4, particle0, particle1, particle2, t4.value() * dt);
        result = std::min(t4.value(), result);
    }
    std::optional<float> t5 = CCDPointTriangle(r2, p1, q1, r1, v6, v1, v2, v3);
    if (t5.has_value())
    {
        ResponsePointTriangle(particle5, particle0, particle1, particle2, t5.value() * dt);
        result = std::min(t5.value(), result);
    }

    auto t6 = CCDEdgeEdge(p1, q1, p2, q2, v1, v2, v4, v5);
    if (t6.has_value())
    {
        ResponseEdgeEdge(particle0, particle1, particle3, particle4, t6.value().first, t6.value().second[0],
                         t6.value().second[1]);
        result = std::min(t6.value().first, result);
    }
    auto t7 = CCDEdgeEdge(p1, r1, p2, q2, v1, v3, v4, v5);
    if (t7.has_value())
    {
        ResponseEdgeEdge(particle0, particle2, particle3, particle4, t7.value().first, t7.value().second[0],
                         t7.value().second[1]);
        result = std::min(t7.value().first, result);
    }
    auto t8 = CCDEdgeEdge(q1, r1, p2, q2, v2, v3, v4, v5);
    if (t8.has_value())
    {
        ResponseEdgeEdge(particle1, particle2, particle3, particle4, t8.value().first, t8.value().second[0],
                         t8.value().second[1]);
        result = std::min(t8.value().first, result);
    }
    spdlog::debug("t8");

    auto t9 = CCDEdgeEdge(p1, q1, p2, r2, v1, v2, v4, v6);
    if (t9.has_value())
    {
        ResponseEdgeEdge(particle1, particle2, particle3, particle5, t9.value().first, t9.value().second[0],
                         t9.value().second[1]);
        result = std::min(t9.value().first, result);
    }
    auto t10 = CCDEdgeEdge(p1, r1, p2, r2, v1, v3, v4, v6);
    if (t10.has_value())
    {
        ResponseEdgeEdge(particle1, particle3, particle3, particle5, t10.value().first, t10.value().second[0],
                         t10.value().second[1]);
        result = std::min(t10.value().first, result);
    }
    auto t11 = CCDEdgeEdge(q1, r1, p2, r2, v2, v3, v4, v6);
    if (t11.has_value())
    {
        ResponseEdgeEdge(particle2, particle3, particle3, particle5, t11.value().first, t11.value().second[0],
                         t11.value().second[1]);
        result = std::min(t11.value().first, result);
    }

    auto t12 = CCDEdgeEdge(p1, q1, q2, r2, v1, v2, v5, v6);
    if (t12.has_value())
    {
        ResponseEdgeEdge(particle1, particle2, particle4, particle5, t12.value().first, t12.value().second[0],
                         t12.value().second[1]);
        result = std::min(t12.value().first, result);
    }
    spdlog::debug("t12");

    auto t13 = CCDEdgeEdge(p1, r1, q2, r2, v1, v3, v5, v6);
    if (t13.has_value())
    {
        ResponseEdgeEdge(particle1, particle3, particle4, particle5, t13.value().first, t13.value().second[0],
                         t13.value().second[1]);
        result = std::min(t13.value().first, result);
    }
    spdlog::debug("t13");

    auto t14 = CCDEdgeEdge(q1, r1, q2, r2, v2, v3, v5, v6);
    if (t14.has_value())
    {
        ResponseEdgeEdge(particle2, particle3, particle4, particle5, t14.value().first, t14.value().second[0],
                         t14.value().second[1]);
        result = std::min(t14.value().first, result);
    }
    spdlog::debug("t14");
    return result;
}