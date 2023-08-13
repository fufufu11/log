#pragma once
/*⽇志消息类主要是封装⼀条完整的⽇志消息所需的内容，其中包括⽇志等级、对应的logger name、打
印⽇志源⽂件的位置信息（包括⽂件名和⾏号）、线程ID、时间戳信息、具体的⽇志信息等内容。*/
#include <iostream>
#include "level.hpp"
#include "util.hpp"
#include <thread>
#include <ctime>
namespace log
{
    struct LogMsg
    {
        size_t _line;         // 行号
        time_t _ctime;        // 时间
        std::thread::id _tid; // 线程ID
        std::string _name;    // 日志器名称
        std::string _file;    // 文件名
        std::string _payload; // 消息主体
        LogLevel::value _level; // 等级

        LogMsg() {}
        LogMsg(LogLevel::value level, size_t line, std::string file,std::string name,std::string payload)
            : _level(level), _line(line), _file(file), _payload(payload), _name(name), _ctime(util::Date::now()),
              _tid(std::this_thread::get_id()) {}
    };
}