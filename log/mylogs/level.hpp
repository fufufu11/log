#pragma once

namespace log
{
    class LogLevel
    {
    public:
        enum class value
        {
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };
        static const char* toString(LogLevel::value level)
        {
            switch(level)
            {
                case LogLevel::value::DEBUG: return "DEBUG";
                case LogLevel::value::INFO: return "INFO";
                case LogLevel::value::WARN: return "WARN";
                case LogLevel::value::ERROR: return "ERROR";
                case LogLevel::value::FATAL: return "FATAL";
                case LogLevel::value::OFF: return "OFF";
                default:return "UNKNOW";
            }
        }
    };
}