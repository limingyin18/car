#include "physics/Particle.hpp"
#include "physics/collision/CCD.hpp"

#include "catch2/catch_approx.hpp"
#include <catch2/catch_test_macros.hpp>

#include <spdlog/spdlog.h>

using namespace std;
using namespace Eigen;
using namespace Catch;
using namespace physics;

TEST_CASE("physics::CCD0", "[physics::CCD]")
{
    float dt = 1.f;

    // first triangle

    Vector3f x0(1.f, 0.25f, 0.f);
    Vector3f x1(1.f, -0.25f, 0.25f);
    Vector3f x2(1.f, -0.25f, -0.25f);

    Vector3f v0(-1.f, 0.f, 0.f);
    Vector3f v1(-1.f, 0.f, 0.f);
    Vector3f v2(-1.f, 0.f, 0.f);

    Particle p0(x0, v0);
    Particle p1(x1, v1);
    Particle p2(x2, v2);

    // second triangle

    Vector3f x3(0.f, 0.5f, 0.f);
    Vector3f x4(0.f, -0.5f, 0.5f);
    Vector3f x5(0.f, -0.5f, -0.5f);

    Vector3f v3(0.f, 0.f, 0.f);
    Vector3f v4(0.f, 0.f, 0.f);
    Vector3f v5(0.f, 0.f, 0.f);

    Particle p3(x3, v3);
    Particle p4(x4, v4);
    Particle p5(x5, v5);

    auto t = CCDTriangleTriangle(p0, p1, p2, p3, p4, p5, dt);
    REQUIRE(t.has_value());
    REQUIRE(t.value() == Approx(1.f));
}

TEST_CASE("physics::CCDPointTriangle", "[CCDPointTriangle]")
{
    Vector3f x0(0.1f, 0.25f, 0.25f);
    Vector3f x1(0.f, 0.f, 0.f);
    Vector3f x2(0.f, 1.f, 0.f);
    Vector3f x3(0.f, 0.f, 1.f);

    Vector3f v0(-1.f, 0.f, 0.f);
    Vector3f v1(0.f, 0.f, 0.f);
    Vector3f v2(0.f, 0.f, 0.f);
    Vector3f v3(0.f, 0.f, 0.f);

    float dt = 1.f;

    auto t = physics::CCDPointTriangle(x0, x1, x2, x3, v0, v1, v2, v3);

    REQUIRE(t.has_value());
    REQUIRE(t.value() == Approx(0.1f));
}