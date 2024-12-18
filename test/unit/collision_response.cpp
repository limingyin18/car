#include "physics/Particle.hpp"
#include "physics/collision/CCD.hpp"

#include "catch2/catch_approx.hpp"
#include <catch2/catch_test_macros.hpp>

#include "physics/collision/Intersection.hpp"
#include "physics/collision/Response.hpp"
#include "tools/Tool.hpp"
#include <spdlog/spdlog.h>

using namespace std;
using namespace Eigen;
using namespace Catch;
using namespace physics;

TEST_CASE("physics::PointTriangleCR0", "[physics::PointTriangleCR0]")
{
    Vector3f x0 = {0.5, 0.5, 0.5};
    Vector3f x1 = {0.888145, 0.397335, -0.000857472};
    Vector3f x2 = {-0.572748, 0.89667, 1.47065};
    Vector3f x3 = {0.709327, 0.108332, 1.46294};

    Vector3f v0 = {0, 0, 0};
    Vector3f v1 = {-0.000674858, -0.00139867, -6.40806e-05};
    Vector3f v2 = {-1.78528e-05, -0.000115299, -0.000222245};
    Vector3f v3 = {-0.00081419, -0.0013972, -0.000537987};

    bool intersect_check = IntersectPointTriangle(x0, x1, x2, x3);
    spdlog::debug("intersect_check: {}", intersect_check);

    Vector3f v0check = {0, 0, 0};
    Vector3f v1check = {0, 0, 0};
    Vector3f v2check = {0, 0, 0};
    Vector3f v3check = {0, 0, 0};

    std::optional<float> t_coplane = FindCollisionTime(x0, x1, x2, x3, v0, v1, v2, v3);
    spdlog::debug("t_coplane: {}", t_coplane.value_or(1000.f));

    std::optional<float> t_coplane_check = FindCollisionTime(x0, x1, x2, x3, v0check, v1check, v2check, v3check);
    spdlog::debug("t_coplane_check: {}", t_coplane_check.value_or(1000.f));

    auto tcheck = CCDPointTriangle(x0, x1, x2, x3, v0check, v1check, v2check, v3check);
    spdlog::debug("tcheck: {}", tcheck.value_or(1000.f));

    auto t = CCDPointTriangle(x0, x1, x2, x3, v0, v1, v2, v3);
    spdlog::debug("t: {}", t.value_or(0.f));

    REQUIRE(t.has_value());
    Particle p(x0, v0, 0);
    Particle p1(x1, v1);
    Particle p2(x2, v2);
    Particle p3(x3, v3);
    ResponsePointTriangle(p, p1, p2, p3, t.value() * 0.016f);
    // ResponsePointTriangleCheck(p, p1, p2, p3, t.value() * 0.016f);

    spdlog::debug("velocity");

    spdlog::debug("x0: {}", Tool::EigenToString(p.pos_));
    spdlog::debug("x1: {}", Tool::EigenToString(p1.pos_));
    spdlog::debug("x2: {}", Tool::EigenToString(p2.pos_));
    spdlog::debug("x3: {}", Tool::EigenToString(p3.pos_));

    spdlog::debug("v0: {}", Tool::EigenToString(p.velocity_));
    spdlog::debug("v1: {}", Tool::EigenToString(p1.velocity_));
    spdlog::debug("v2: {}", Tool::EigenToString(p2.velocity_));
    spdlog::debug("v3: {}", Tool::EigenToString(p3.velocity_));

    x0 = p.pos_;
    x1 = p1.pos_;
    x2 = p2.pos_;
    x3 = p3.pos_;

    v0 = p.velocity_ * 0.016f;
    v1 = p1.velocity_ * 0.016f;
    v2 = p2.velocity_ * 0.016f;
    v3 = p3.velocity_ * 0.016f;
    auto tt = CCDPointTriangle(x0, x1, x2, x3, v0, v1, v2, v3);
    spdlog::debug("tt: {}", tt.value_or(0.f));
    REQUIRE(!tt.has_value());
}