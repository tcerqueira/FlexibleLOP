#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::s_mesLogger;

void Log::init()
{
    s_mesLogger = spdlog::stdout_color_mt("MES");
    s_mesLogger->set_pattern("%^[%T] %n: %v%$");
    s_mesLogger->set_level(spdlog::level::trace);
}
