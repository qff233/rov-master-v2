#include "log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

Log::Log()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    m_driverLogger = spdlog::stdout_color_mt("Driver");
    m_driverLogger->set_level(spdlog::level::trace);

    m_userLogger = spdlog::stdout_color_mt("User");
    m_userLogger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger> &Log::getUserLogger()
{
    return m_userLogger;
}

std::shared_ptr<spdlog::logger> &Log::getDriverLogger()
{
    return m_driverLogger;
}