#pragma once
#include "util.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
#include <memory>
namespace log
{
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        virtual ~LogSink(){}
        virtual void log(const char* data,size_t len) = 0;
    };

    //落地方向：标准输出
    class StdoutSink : public LogSink
    {
    public:
        virtual void log(const char* data,size_t len)
        {
            std::cout.write(data,len);
        }
    };
    //落地方向：指定文件
    class FileSink : public LogSink
    {
    public:
        //构造时传入文件，并打开文件名，将操作句柄管理起来
        FileSink(const std::string& pathname):_pathname(pathname)
        {
            //创建日志文件所在的目录
            util::File::create_directory(util::File::path(pathname));
            //创建并打开日志文件
            _ofs.open(_pathname,std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        virtual void log(const char* data,size_t len)
        {
            _ofs.write(data,len);
            assert(_ofs.good());
        }
    private:
        std::string _pathname;
        std::ofstream _ofs;
    };
    class RollBySizeSink : public LogSink
    {
    public:
        RollBySizeSink(const std::string &basename, size_t max_size)
            : _basename(basename), _max_fsize(max_size), _cur_fsize(0),_namecnt(0)
        {
            std::string pathname = createNewFile();
            // 创建日志文件所在的目录
            util::File::create_directory(util::File::path(pathname));
            // 创建并打开日志文件
            _ofs.open(pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        virtual void log(const char* data,size_t len)
        {
            if(_max_fsize <= _cur_fsize)
            {
                _ofs.close();
                std::string pathname = createNewFile();
                _ofs.open(pathname, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
                _cur_fsize = 0;
            }
            _ofs.write(data,len);
            _cur_fsize += len;
            assert(_ofs.good());
        }
    private:
        std::string createNewFile()
        {
            //获取系统时间，以时间来构造文件名扩展名
            time_t t = util::Date::now();
            struct tm lt;
            localtime_r(&t,&lt);
            std::stringstream filename;
            filename << _basename;
            filename << lt.tm_year + 1900;
            filename << lt.tm_mon + 1;
            filename << lt.tm_mday;
            filename << lt.tm_hour;
            filename << lt.tm_min;
            filename << lt.tm_sec;
            filename << "-";
            filename << _namecnt++;
            filename << ".log";
            return filename.str();
        }
    private:
        size_t _namecnt;
        std::string _basename;
        std::ofstream _ofs;
        size_t _max_fsize;
        size_t _cur_fsize;
    };
    class SinkFactory
    {
    public:
        template<class SinkType,class ...Args>
        static LogSink::ptr create(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };
}