#include "Intersection.hpp"
#include "Eigen/Core"
#include "math/cubicSolver_adsk.h"
#include <optional>

using namespace physics;
using namespace Eigen;
using namespace std;

bool physics::IntersectTriangleTriangle(Vector3f const &p1, Vector3f const &q1, Vector3f const &r1, Vector3f const &p2,
                                        Vector3f const &q2, Vector3f const &r2)
{
    Vector3f n1 = (q1 - p1).cross(r1 - p1);
    Vector3f n2 = (q2 - p2).cross(r2 - p2);

    auto IntersectEdgeTriangle = [](Vector3f const &xa, Vector3f const &xb, Vector3f const &x0, Vector3f const &x1,
                                    Vector3f const &x2, Vector3f const &n) -> bool {
        float t = IntersectEdgePlane(xa, xb, x0, n);
        if (t >= 0.f && t <= 1.f)
        {
            Vector3f xt = xa + t * (xb - xa);

            if (IntersectPointTriangle(xt, x0, x1, x2))
            {
                return true;
            }
        }

        return false;
    };

    if (IntersectEdgeTriangle(p1, q1, p2, q2, r2, n2))
    {
        return true;
    }
    if (IntersectEdgeTriangle(p1, r1, p2, q2, r2, n2))
    {
        return true;
    }
    if (IntersectEdgeTriangle(q1, r1, p2, q2, r2, n2))
    {
        return true;
    }
    if (IntersectEdgeTriangle(p2, q2, p1, q1, r1, n1))
    {
        return true;
    }
    if (IntersectEdgeTriangle(p2, r2, p1, q1, r1, n1))
    {
        return true;
    }
    if (IntersectEdgeTriangle(q2, r2, p1, q1, r1, n1))
    {
        return true;
    }
    return false;
}

bool physics::IntersectPointTriangle(Eigen::Vector3f const &p, Eigen::Vector3f const &x0, Eigen::Vector3f const &x1,
                                     Eigen::Vector3f const &x2)
{
    Vector3f n = (x1 - x0).cross(x2 - x0).normalized();
    Vector3f x0p = x0 - p;
    Vector3f x1p = x1 - p;
    Vector3f x2p = x2 - p;

    if (n.dot(x0p.cross(x1p)) > 0 && n.dot(x1p.cross(x2p)) > 0 && n.dot(x2p.cross(x0p)) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

float physics::IntersectEdgePlane(Eigen::Vector3f const &xa, Eigen::Vector3f const &xb, Eigen::Vector3f const &x0,
                                  Eigen::Vector3f const &n)
{
    float t = 0.f;

    Vector3f x0a = xa - x0;
    Vector3f xba = xb - xa;
    t = n.dot(x0a) / n.dot(xba);
    return t;
}

std::optional<std::array<float, 2>> physics::IntersectEdgeEdge(Eigen::Vector3f const &x0, Eigen::Vector3f const &x1,
                                                               Eigen::Vector3f const &x2, Eigen::Vector3f const &x3)
{
    float s, t;
    Vector3f c1, c2;
    float dist = CloestPtSegmentSegment(x0, x1, x2, x3, s, t, c1, c2);
    if (dist < 1e-6)
    {
        return std::array<float, 2>{s, t};
    }
    return std::nullopt;
}

float physics::CloestPtSegmentSegment(const Eigen::Vector3f &p1, const Eigen::Vector3f &q1, const Eigen::Vector3f &p2,
                                      const Eigen::Vector3f &q2, float &s, float &t, Eigen::Vector3f &c1,
                                      Eigen::Vector3f &c2)
{
    Vector3f d1 = q1 - p1;
    Vector3f d2 = q2 - p2;
    Vector3f r = p1 - p2;
    float a = d1.dot(d1);
    float e = d2.dot(d2);
    float f = d2.dot(r);

    if (a <= 1e-6 && e <= 1e-6)
    {
        s = t = 0.0f;
        c1 = p1;
        c2 = p2;
        return (c1 - c2).norm();
    }

    if (a <= 1e-6)
    {
        s = 0.0f;
        t = f / e;
        t = std::clamp(t, 0.0f, 1.0f);
    }
    else
    {
        float c = d1.dot(r);
        if (e <= 1e-6)
        {
            t = 0.0f;
            s = std::clamp(-c / a, 0.0f, 1.0f);
        }
        else
        {
            float b = d1.dot(d2);
            float denom = a * e - b * b;

            if (denom != 0.0f)
            {
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }
            else
            {
                s = 0.0f;
            }

            float tnom = b * s + f;
            if (tnom < 0.0f)
            {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (tnom > e)
            {
                t = 1.0f;
                s = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
            else
            {
                t = tnom / e;
            }
        }
    }

    c1 = p1 + s * d1;
    c2 = p2 + t * d2;
    return (c1 - c2).norm();
}

int physics::cubic_solver(float a, float b, float c, float d, float *roots)
{
    // using Autodesk's Cubic Solver as mine is not working properly every time...
    float poly[4];
    poly[3] = a;
    poly[2] = b;
    poly[1] = c;
    poly[0] = d;
    float dt = 0.05f;
    init_tolerance();
    int numRoots = polyZeroes(poly, 3, 0.0 - dt, 1, 1.0 + dt, 1, roots);
    numRoots = numRoots == -1 ? 0 : numRoots;
    return numRoots;
}