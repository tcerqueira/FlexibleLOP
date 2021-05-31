#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::s_mesLogger;
std::shared_ptr<spdlog::logger> Log::s_dbLogger;

void Log::init()
{
    // MES logger
    s_mesLogger = spdlog::stdout_color_mt("MES");
    s_mesLogger->set_pattern("%^[%T] %n: %v%$");
    s_mesLogger->set_level(spdlog::level::trace);
    // DB logger
    s_dbLogger = spdlog::stdout_color_mt("DB");
    s_dbLogger->set_pattern("%^[%T] %n: %v%$");
    s_dbLogger->set_level(spdlog::level::trace);
}
