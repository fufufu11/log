#pragma once
#include "logger.hpp"

namespace log
{
    Logger::ptr getLogger(const std::string &name)
    {
        return LoggerManager::getInstance().getLogger(name);
    }
    Logger::ptr rootLogger()
    {
        return LoggerManager::getInstance().rootLogger();
    }

#define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define error(fmt, ...) error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define DEBUG(fmt, ...) log::rootLogger()->debug(fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) log::rootLogger()->info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) log::rootLogger()->warn(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) log::rootLogger()->error(fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) log::rootLogger()->fatal(fmt, ##__VA_ARGS__)

// #define LOGD(fmt, ...) LOG_DEBUG(log::rootLogger(), fmt, ##__VA_ARGS__)
// #define LOGI(fmt, ...) LOG_INFO(log::rootLogger(), fmt, ##__VA_ARGS__)
// #define LOGW(fmt, ...) LOG_WARN(log::rootLogger(), fmt, ##__VA_ARGS__)
// #define LOGE(fmt, ...) LOG_ERROR(log::rootLogger(), fmt, ##__VA_ARGS__)
// #define LOGF(fmt, ...) LOG_FATAL(log::rootLogger(), fmt, ##__VA_ARGS__)

}