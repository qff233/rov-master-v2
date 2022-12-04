#ifndef __USER_LOG_H__
#define __USER_LOG_H__

#include <memory>
#include <spdlog/spdlog.h>

#include "utils.h"

#define DRIVER_LOG_ERROR(...) Global<Log>::Get()->getDriverLogger()->error(__VA_ARGS__);
#define DRIVER_LOG_WARN(...)  Global<Log>::Get()->getDriverLogger()->warn(__VA_ARGS__);
#define DRIVER_LOG_INFO(...)  Global<Log>::Get()->getDriverLogger()->info(__VA_ARGS__);
#define DRIVER_LOG_DEBUG(...) Global<Log>::Get()->getDriverLogger()->debug(__VA_ARGS__);

#define LOG_ERROR(...) Global<Log>::Get()->getUserLogger()->error(__VA_ARGS__);
#define LOG_WARN(...)  Global<Log>::Get()->getUserLogger()->warn(__VA_ARGS__);
#define LOG_INFO(...)  Global<Log>::Get()->getUserLogger()->info(__VA_ARGS__);
#define LOG_DEBUG(...) Global<Log>::Get()->getUserLogger()->debug(__VA_ARGS__);

class Log
{
public:
    Log();

    std::shared_ptr<spdlog::logger> &getUserLogger();
    std::shared_ptr<spdlog::logger> &getDriverLogger();

private:
    std::shared_ptr<spdlog::logger> m_userLogger;
    std::shared_ptr<spdlog::logger> m_driverLogger;
};

#endif