#pragma once

#include "l_comms.h"
#include <spdlog/spdlog.h>
#include "spdlog/fmt/ostr.h"

class Log
{
public:
    static void init();
    inline static std::shared_ptr<spdlog::logger>& getLogger() { return s_mesLogger; }
private:
    static std::shared_ptr<spdlog::logger> s_mesLogger;
};

// Log macros
#define MES_TRACE(...)      ::Log::getLogger()->trace(__VA_ARGS__)
#define MES_INFO(...)       ::Log::getLogger()->info(__VA_ARGS__)
#define MES_WARN(...)       ::Log::getLogger()->warn(__VA_ARGS__)
#define MES_ERROR(...)      ::Log::getLogger()->error(__VA_ARGS__)
#define MES_FATAL(...)      ::Log::getLogger()->critical(__VA_ARGS__)