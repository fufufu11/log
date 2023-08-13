#pragma once
#include <memory>
#include <mutex>
#include <thread>
#include "format.hpp"
#include "sink.hpp"
#include "util.hpp"
#include "message.hpp"
#include "level.hpp"
#include "looper.hpp"
#include <atomic>
#include <stdarg.h>
#include <vector>
#include <unordered_map>
namespace log
{
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string &logger_name,
        LogLevel::value level,
        std::vector<LogSink::ptr> &sinks,
        Formatter::ptr formatter
        ) : _logger_name(logger_name), _limit_level(level), _formatter(formatter),_sinks(sinks.begin(), sinks.end()){}
        // 完成构造日志消息对象过程并进行格式化，得到格式化后的日志消息字符串，然后进行落地输出
        const std::string& name() {return _logger_name;}
        void debug(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if(LogLevel::value::DEBUG < _limit_level) return;
            va_list al;
            va_start(al,fmt);
            char* tmp;
            int ret = vasprintf(&tmp,fmt.c_str(),al);
            if(ret == -1) 
            {
                std::cout << "vasprintf error" << std::endl;
                return;
            }
            LogMsg msg(LogLevel::value::DEBUG,line,file,_logger_name,tmp);
            std::stringstream ss;
            _formatter->format(ss,msg);
            log(ss.str().c_str(),ss.str().size());
            va_end(al);
            free(tmp);
        }
        void info(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if (LogLevel::value::INFO < _limit_level)
                return;
            va_list al;
            va_start(al, fmt);
            char* tmp;
            int ret = vasprintf(&tmp, fmt.c_str(), al);
            if (ret == -1)
            {
                std::cout << "vasprintf error" << std::endl;
                return;
            }
            LogMsg msg(LogLevel::value::INFO, line, file, _logger_name, tmp);
            std::stringstream ss;
            _formatter->format(ss,msg);
            log(ss.str().c_str(),ss.str().size());
            va_end(al);
            free(tmp);
        }
        void warn(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if (LogLevel::value::WARN < _limit_level)
                return;
            va_list al;
            va_start(al, fmt);
            char *tmp;
            int ret = vasprintf(&tmp, fmt.c_str(), al);
            if (ret == -1)
            {
                std::cout << "vasprintf error" << std::endl;
                return;
            }
            LogMsg msg(LogLevel::value::WARN, line, file, _logger_name, tmp);
            std::stringstream ss;
            _formatter->format(ss,msg);
            log(ss.str().c_str(),ss.str().size());
            va_end(al);
            free(tmp);
        }
        void error(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if (LogLevel::value::ERROR < _limit_level)
                return;
            va_list al;
            va_start(al, fmt);
            char *tmp;
            int ret = vasprintf(&tmp, fmt.c_str(), al);
            if (ret == -1)
            {
                std::cout << "vasprintf error" << std::endl;
                return;
            }
            LogMsg msg(LogLevel::value::ERROR, line, file,_logger_name, tmp);
            std::stringstream ss;
            _formatter->format(ss,msg);
            log(ss.str().c_str(),ss.str().size()); 
            va_end(al);
            free(tmp);
        }
        void fatal(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if (LogLevel::value::FATAL < _limit_level)
                return;
            va_list al;
            va_start(al, fmt);
            char *tmp;
            int ret = vasprintf(&tmp, fmt.c_str(), al);
            if (ret == -1)
            {
                std::cout << "vasprintf error" << std::endl;
                return;
            }
            LogMsg msg(LogLevel::value::FATAL, line, file,_logger_name, tmp);
            std::stringstream ss;
            _formatter->format(ss,msg);
            log(ss.str().c_str(),ss.str().size());
            va_end(al);
            free(tmp);
        }

    protected:
        // 抽象接口完成实际的落地输出，不同的日志器会有不同的实际落地方式
        virtual void log(const char *data, size_t len) = 0;

    protected:
        std::mutex _mutex;
        std::string _logger_name;
        std::atomic<LogLevel::value> _limit_level;
        Formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };
    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string& name,
            LogLevel::value level,
            std::vector<LogSink::ptr> &sinks,
            Formatter::ptr& formatter):
            Logger(name,level,sinks,formatter){}
    protected:
        // 同步日志器，是将日志直接通过落地模块句柄进行日志落地
        void log(const char* data,size_t len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if(_sinks.empty()) return;
            for(auto &sink: _sinks)
            {
                sink->log(data,len);
            }
        }
    };
    //抽象一个日志器建造者类(完成日志器对象所需部件的构建&日志器的构建)
      // 1.设置日志器类型 2.将不同类型日志器的创建放到同一个日志器建造类中完成
    enum  LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC
    };
    class LoggerBuilder
    {
    public:
        LoggerBuilder():_logger_type(LoggerType::LOGGER_SYNC),_limit_level(LogLevel::value::DEBUG)
        ,_looper_type(AsyncType::ASYINC_SAFR){}
        void buildLoggerType(LoggerType type) {_logger_type = type;}
        void buildEnableUnSafeAsync() {_looper_type = AsyncType::ASYNC_UNSAFE;}
        void buildLoggerName(const std::string &name){_logger_name = name;}
        void buildLoggerLevel(LogLevel::value level){_limit_level = level;}
        void buildFormatter(const std::string &pattern)
        {
            _formatter = std::make_shared<Formatter> (pattern);
        }
        template<typename SinkType,typename ...Args>
        void buildSink(Args &&...args)
        {
            LogSink::ptr psink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }
        virtual Logger::ptr build() = 0;
    protected:
        AsyncType _looper_type;
        LoggerType _logger_type;
        std::string _logger_name;
        LogLevel::value _limit_level;
        Formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };
    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(const std::string& name,
            LogLevel::value level,
            std::vector<LogSink::ptr> &sinks,
            Formatter::ptr& formatter,
            AsyncType looper_type):
            Logger(name,level,sinks,formatter),
            _looper(std::make_shared<AsyncLooper> (std::bind(&AsyncLogger::realLog,this,std::placeholders::_1),looper_type)){}
        void log(const char* data,size_t len)//将数据写入缓冲区
        {
            _looper->push(data,len);
        }
        //设计一个实际落地函数（将缓冲区中的数据落地）
        void realLog(Buffer& buf)
        {
            if(_sinks.empty()) return;
            for(auto &sink: _sinks)
            {
                sink->log(buf.begin(),buf.readAbleSize());
            }
        }
    private:
        AsyncLooper::ptr _looper;
    };
    //派生出具体的建造者类---局部日志器的建造者&全局的日志器建造者（后边添加了全局单例管理器之后，将日志器添加全局）
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(_logger_name.empty() == false);
            if(_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
                buildSink<StdoutSink>();
            }
            if(_logger_type == LoggerType::LOGGER_ASYNC)
            {
                return std::make_shared<AsyncLogger>(_logger_name,_limit_level,_sinks,_formatter,_looper_type);
            }
            return std::make_shared<SyncLogger>(_logger_name,_limit_level,_sinks,_formatter);
        }
    };
    class LoggerManager
    {
    public:
        static LoggerManager& getInstance()
        {
            static LoggerManager eton;
            return eton;
        }
        void addLogger(Logger::ptr &logger)
        {
            if(hasLogger(logger->name())) return;
            std::unique_lock<std::mutex> lock(_mutex);
            _loggers.insert(std::make_pair(logger->name(),logger));
        }
        bool hasLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if(it == _loggers.end()) return false;
            return true;
        }
        Logger::ptr getLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if(it == _loggers.end())
            {
                return Logger::ptr();
            }
            return it->second;
        }
        Logger::ptr rootLogger() {return _root_logger;}
    private:
        LoggerManager()
        {
            std::unique_ptr<log::LoggerBuilder> builder(new log::LocalLoggerBuilder());
            builder->buildLoggerName("root");
            _root_logger = builder->build();
            _loggers.insert(std::make_pair("root",_root_logger));
        }
    private:
        std::mutex _mutex;
        Logger::ptr _root_logger;//默认日志器
        std::unordered_map<std::string,Logger::ptr> _loggers;
    };

    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        using ptr = std::shared_ptr<GlobalLoggerBuilder>;
        Logger::ptr build() override
        {
            assert(_logger_name.empty() == false);
            if(_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
                buildSink<StdoutSink>();
            }
            Logger::ptr logger;
            if(_logger_type == LoggerType::LOGGER_ASYNC)
            {
               logger =  std::make_shared<AsyncLogger>(_logger_name,_limit_level,_sinks,_formatter,_looper_type);
            }
            else
            {
                logger = std::make_shared<SyncLogger>(_logger_name,_limit_level,_sinks,_formatter);
            }
            LoggerManager::getInstance().addLogger(logger);
            return logger;
        }
    };
};