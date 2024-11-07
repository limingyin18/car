#include "math/Math.hpp"
#include "math/Geometry.hpp"

#include "catch2/catch_approx.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace std;
using namespace Eigen;
using namespace Catch;

TEST_CASE("math::NormalTriangle0", "[math::NormalTriangle]")
{
    Vector3f a(0.f, 0.f, 0.f);
    Vector3f b(1.f, 0.f, 0.f);
    Vector3f c(0.f, 1.f, 0.f);

    auto n = math::NormalTriangle(a, b, c);

    REQUIRE(n.x() == Approx(0.f));
    REQUIRE(n.y() == Approx(0.f));
    REQUIRE(n.z() == Approx(1.f));
}

TEST_CASE("math::Barycentric0", "[math::Barycentric]")
{
    Vector3f p(0.5f, 0.5f, 0.f);
    Vector3f a(0.f, 0.f, 0.f);
    Vector3f b(1.f, 0.f, 0.f);
    Vector3f c(0.f, 1.f, 0.f);

    auto barycentric = math::Barycentric(p, a, b, c);

    REQUIRE(barycentric[0] == Approx(0.5f));
    REQUIRE(barycentric[1] == Approx(0.5f));
    REQUIRE(barycentric[2] == Approx(0.f));
}