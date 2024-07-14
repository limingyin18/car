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
        std::cout << "Hello World!" << std::endl;

        App app;

        app.Init();

        app.Run();

        app.Destroy();
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}