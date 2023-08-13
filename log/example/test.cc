#include "../mylogs/logs.hpp"
#include <unistd.h>

int main()
{
    /*
 系统的默认⽇志输出格式：%d{%H:%M:%S}%T[%t]%T[%p]%T[%c]%T%f:%l%T%m%n
13:26:32 [2343223321] [FATAL] [root] main.c:76套接字创建失败\n
%d{%H:%M:%S}：表⽰⽇期时间，花括号中的内容表⽰⽇期时间的格式。
%T：表⽰制表符缩进。
%t：表⽰线程ID
%p：表⽰⽇志级别
%c：表⽰⽇志器名称，不同的开发组可以创建⾃⼰的⽇志器进⾏⽇志输出，⼩组之间互不影响。
%f：表⽰⽇志输出时的源代码⽂件名。
%l：表⽰⽇志输出时的源代码⾏号。
%m：表⽰给与的⽇志有效载荷数据
%n：表⽰换⾏
  */
    int error = -1;
    if (error == -1)
    {
        ERROR("%s", "这是一个测试");//默认只往终端打印
        //打印结果:
        //[22:18:27][139977913136960][root][test.cc:22][ERROR]    这是一个测试
    }

    //以下是往终端、文件、滚动文件（根据文件大小会切换文件打印）打印的例子
    std::unique_ptr<log::LoggerBuilder> builder(new log::GlobalLoggerBuilder());
    // std::unique_ptr<log::LoggerBuilder> builder(new log::LocalLoggerBuilder());
    builder->buildLoggerName("async_logger");//异步
    builder->buildLoggerLevel(log::LogLevel::value::UNKNOW);//高于UNKNOW等级才打印
    builder->buildFormatter("[%c][%f:%l]%m%n");
    builder->buildSink<log::StdoutSink>();//往标准输出打印，也即终端打印
    builder->buildSink<log::FileSink>("./logfile/async.log");//往指定文件打印，文件不存在会创建文件
    builder->buildSink<log::RollBySizeSink>("./logs/roll-", 10 * 1024 * 1024);//往指定文件打印，超过10MB后切换文件
    builder->buildLoggerType(log::LoggerType::LOGGER_ASYNC);//设置为异步类型
   // builder->buildEnableUnSafeAsync();//这里是开启无限扩容，不阻塞
    builder->build();
    log::Logger::ptr logger = log::LoggerManager::getInstance().getLogger("async_logger");
    logger->debug("%s", "测试日志");
    logger->info("%s", "测试日志");
    logger->warn("%s", "测试日志");
    logger->error("%s", "测试日志");
    logger->fatal("%s", "测试日志");
    size_t count = 0;
    while (count < 500000)
    {
        logger->fatal("测试日志-%d", count++);
    }
    return 0;
}
