#include "Geometry.hpp"
#include "Eigen/Core"

using namespace math;
using namespace Eigen;
using namespace std;

std::array<float, 3> math::Barycentric(const Vector3f &p, const Vector3f &a, const Vector3f &b, const Vector3f &c)
{
    std::array<float, 3> barycentric;

    Vector3f n = NormalTriangle(a, b, c);

    float a0 = (b-p).cross(c-p).dot(n);
    float a1 = (c-p).cross(a-p).dot(n);
    float a2 = (a-p).cross(b-p).dot(n);

    float sum = a0 + a1 + a2;

    barycentric[0] = a0 / sum;
    barycentric[1] = a1 / sum;
    barycentric[2] = a2 / sum;

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

float math::CloestPtSegmentSegment(const Eigen::Vector3f &p1, const Eigen::Vector3f &q1, const Eigen::Vector3f &p2,
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
                s = 0.5f;
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

Vector3f math::ClosestPtPointPlane(const Eigen::Vector3f &q, const Eigen::Vector3f &p, const Eigen::Vector3f &n)
{
    return q - n.dot(q - p) * n;
}