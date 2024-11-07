#define CATCH_CONFIG_RUNNER
#include "catch2/catch_session.hpp"
#include <catch2/catch_test_macros.hpp>
#include <spdlog/spdlog.h>

using namespace std;
using namespace Catch;

int main(int argc, char *argv[])
{
    // global setup...
#ifndef NDEBUG
    spdlog::default_logger()->set_level(spdlog::level::debug);
#endif

    spdlog::info("========== test start ==========");

    int result = Catch::Session().run(argc, argv);

    // global clean-up...

    return result;
}