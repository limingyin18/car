#include <iostream>
#include <spdlog/spdlog.h>

#include "App.hpp"

using namespace std;

int main(int argc, char** argv)
{

#ifndef NDEBUG
    spdlog::default_logger()->set_level(spdlog::level::debug);
#endif

    spdlog::info("app start");

    try
    {
        App app;

        app.Init();

        app.Run();

        app.Destroy();
    }
    catch (const exception& e)
    {
        spdlog::error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}