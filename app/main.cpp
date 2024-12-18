#include <fstream>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "game/Game.hpp"

using namespace std;

std::shared_ptr<spdlog::logger> logger;
void InitLOG(int argc, char **argv);

int main(int argc, char **argv)
{
    InitLOG(argc, argv);

    try
    {
        Game &app = Game::GetInstance();

        app.Init();

        app.Run();

        app.Destroy();
    }
    catch (const exception &e)
    {
        spdlog::error(e.what());
        spdlog::drop_all();
        return EXIT_FAILURE;
    }

    // spdlog::drop_all();
    return EXIT_SUCCESS;
}

void InitLOG(int argc, char **argv)
{
    if (argc > 1 && string(argv[1]) == "log")
    {
        string filename_log = "cloth_simulation.log";
        logger = spdlog::basic_logger_mt("spdlog", filename_log);
        spdlog::set_default_logger(logger);

        std::ofstream ofs(filename_log, std::ofstream::out | std::ofstream::trunc);
        ofs.close();
    }
    if (argc > 2 && string(argv[2]) == "debug")
    {
        spdlog::default_logger()->set_level(spdlog::level::debug);
    }
    else
    {
        spdlog::default_logger()->set_level(spdlog::level::debug);
#ifdef NDEBUG
        spdlog::default_logger()->set_level(spdlog::level::info);
#endif
    }
}