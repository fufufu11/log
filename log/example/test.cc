#include "../mylogs/logs.hpp"
#include <unistd.h>
void test_log()
{
    // log::Logger::ptr logger = log::LoggerManager::getInstance().getLogger("async_logger");
    // logger->debug("%s", "测试日志");
    // logger->info("%s", "测试日志");
    // logger->warn("%s", "测试日志");
    // logger->error("%s", "测试日志");
    // logger->fatal("%s", "测试日志");
    // size_t count = 0;
    // while (count < 500000)
    // {
    //     logger->fatal("测试日志-%d", count++);
    // }
    //log::Logger::ptr logger = log::LoggerManager::getInstance().getLogger("async_logger");
    DEBUG("%s", "测试日志");
    INFO("%s", "测试日志");
    WARN("%s", "测试日志");
    ERROR("%s", "测试日志");
    FATAL("%s", "测试日志");
    size_t count = 0;
    while (count < 500000)
    {
        FATAL("测试日志-%d", count++);
    }
}
int main()
{
    std::unique_ptr<log::LoggerBuilder> builder(new log::GlobalLoggerBuilder());
    //std::unique_ptr<log::LoggerBuilder> builder(new log::LocalLoggerBuilder());
    builder->buildLoggerName("async_logger");
    builder->buildLoggerLevel(log::LogLevel::value::UNKNOW);
    builder->buildFormatter("[%c][%f:%l]%m%n");
    builder->buildSink<log::StdoutSink>();
    builder->buildSink<log::FileSink>("./logfile/async.log");
    builder->buildLoggerType(log::LoggerType::LOGGER_ASYNC);
    builder->buildEnableUnSafeAsync();
    //log::Logger::ptr logger = builder->build();
    builder->build();
    test_log();
    return 0;
}