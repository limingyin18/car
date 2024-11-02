#include <iostream>

#include "physics/collision/CCD.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

using namespace std;
using namespace Eigen;

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
    if (t.has_value())
    {
        std::cout << "t: " << t.value() << std::endl;
    }
    else
    {
        std::cout << "No intersection" << std::endl;
    }

    REQUIRE(t.has_value());
    REQUIRE(t.value() == 0.1f);
}
