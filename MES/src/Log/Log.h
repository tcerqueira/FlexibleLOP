#pragma once

#include "l_comms.h"
#include <spdlog/spdlog.h>
#include "spdlog/fmt/ostr.h"

class Log
{
public:
    static void init();
    inline static std::shared_ptr<spdlog::logger>& getMESLogger() { return s_mesLogger; }
    inline static std::shared_ptr<spdlog::logger>& getDBLogger() { return s_dbLogger; }
private:
    static std::shared_ptr<spdlog::logger> s_mesLogger;
    static std::shared_ptr<spdlog::logger> s_dbLogger;
};

// Log macros
// MES
#ifdef DEBUG
#define MES_TRACE(...)      ::Log::getMESLogger()->trace(__VA_ARGS__)
#else
#define MES_TRACE(...)
#endif
#define MES_INFO(...)       ::Log::getMESLogger()->info(__VA_ARGS__)
#define MES_WARN(...)       ::Log::getMESLogger()->warn(__VA_ARGS__)
#define MES_ERROR(...)      ::Log::getMESLogger()->error(__VA_ARGS__)
#define MES_FATAL(...)      ::Log::getMESLogger()->critical(__VA_ARGS__)
// DB
#ifdef DEBUG
#define DB_TRACE(...)      ::Log::getDBLogger()->trace(__VA_ARGS__)
#else
#define DB_TRACE(...)
#endif
#define DB_INFO(...)       ::Log::getDBLogger()->info(__VA_ARGS__)
#define DB_WARN(...)       ::Log::getDBLogger()->warn(__VA_ARGS__)
#define DB_ERROR(...)      ::Log::getDBLogger()->error(__VA_ARGS__)
#define DB_FATAL(...)      ::Log::getDBLogger()->critical(__VA_ARGS__)
