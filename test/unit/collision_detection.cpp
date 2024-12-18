#include "physics/Particle.hpp"
#include "physics/collision/CCD.hpp"

#include "catch2/catch_approx.hpp"
#include <catch2/catch_test_macros.hpp>

#include "tools/Tool.hpp"
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

TEST_CASE("physics::CCD1", "[physics::CCD1]")
{
    float dt = 0.016f;

    // first triangle

    Vector3f x0(-0.004f, 0.5f, 0.5f);
    Vector3f x1(-0.004f, 1.f, 0.f);
    Vector3f x2(-0.004f, 0.5f, -0.5f);

    Vector3f v0(1.f, 0.f, 0.f);
    Vector3f v1(1.f, 0.f, 0.f);
    Vector3f v2(1.f, 0.f, 0.f);

    Particle p0(x0, v0);
    Particle p1(x1, v1);
    Particle p2(x2, v2);

    // second triangle

    Vector3f x3(0.004f, 0.5f, 0.5f);
    Vector3f x4(0.004f, 1.f, 0.f);
    Vector3f x5(0.004f, 0.5f, -0.5f);

    Vector3f v3(-1.f, 0.f, 0.f);
    Vector3f v4(-1.f, 0.f, 0.f);
    Vector3f v5(-1.f, 0.f, 0.f);

    Particle p3(x3, v3);
    Particle p4(x4, v4);
    Particle p5(x5, v5);

    auto t = CCDTriangleTriangle(p0, p1, p2, p3, p4, p5, dt);

    spdlog::info("particles_a_");
    spdlog::info("{}", Tool::EigenToString(p0.velocity_));
    spdlog::info("{}", Tool::EigenToString(p1.velocity_));
    spdlog::info("{}", Tool::EigenToString(p2.velocity_));

    spdlog::info("particles_b_");
    spdlog::info("{}", Tool::EigenToString(p3.velocity_));
    spdlog::info("{}", Tool::EigenToString(p4.velocity_));
    spdlog::info("{}", Tool::EigenToString(p5.velocity_));

    REQUIRE(t.has_value());
}

TEST_CASE("physics::CCD2", "[physics::CCD2]")
{
    float dt = 0.016f;

    // first triangle

    Vector3f x0(0.888145, 0.397335, -0.000857472);
    Vector3f x1(-0.572748, 0.89667, 1.47065);
    Vector3f x2(0.709327, 0.108332, 1.46294);

    Vector3f v0(-0.0421786, -0.0874168, -0.00400504);
    Vector3f v1(-0.0011158, -0.0072062, -0.0138903);
    Vector3f v2(-0.0508869, -0.0873251, -0.0336242);

    Particle p0(x0, v0);
    Particle p1(x1, v1);
    Particle p2(x2, v2);

    // second triangle

    Vector3f x3(0.5, 0.5, -0.5);
    Vector3f x4(-0.5, 0.5, 0.5);
    Vector3f x5(0.5, 0.5, 0.5);

    Vector3f v3(0.f, 0.f, 0.f);
    Vector3f v4(0.f, 0.f, 0.f);
    Vector3f v5(0.f, 0.f, 0.f);

    Particle p3(x3, v3, 0.f);
    Particle p4(x4, v4, 0.f);
    Particle p5(x5, v5, 0.f);

    auto t = CCDTriangleTriangle(p0, p1, p2, p3, p4, p5, dt);

    // spdlog::info("particles_a_");
    // spdlog::info("{}", Tool::EigenToString(p0.velocity_));
    // spdlog::info("{}", Tool::EigenToString(p1.velocity_));
    // spdlog::info("{}", Tool::EigenToString(p2.velocity_));

    // spdlog::info("particles_b_");
    // spdlog::info("{}", Tool::EigenToString(p3.velocity_));
    // spdlog::info("{}", Tool::EigenToString(p4.velocity_));
    // spdlog::info("{}", Tool::EigenToString(p5.velocity_));

    REQUIRE(t.has_value());

    auto tt = physics::CCDTriangleTriangleCheck(p0, p1, p2, p3, p4, p5, 0.016f);
    REQUIRE(!tt.has_value());
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