#include <fstream>
#include <iostream>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>


#include "App.hpp"

using namespace std;

void clear_log_file(const std::string &filename)
{
    std::ofstream ofs(filename, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

int main(int argc, char **argv)
{
    std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("spdlog", "cloth_simulation.log");
    spdlog::set_default_logger(logger);
    // Clear the log file
    clear_log_file("cloth_simulation.log");

#ifndef NDEBUG
    spdlog::default_logger()->set_level(spdlog::level::debug);
#endif

    spdlog::info("========== app start ==========");

    try
    {
        App app;

        app.Init();

        app.Run();

        app.Destroy();
    }
    catch (const exception &e)
    {
        spdlog::error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}