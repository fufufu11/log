#pragma once
/*
    通用的功能类
    1.获取系统时间
    2.判断文件是否存在
    3.获取文件路径
    4.创建目录
*/
#include <iostream>
#include <sys/stat.h>
#include <ctime>
namespace log
{
    namespace util
    {
        class Date
        {
        public:
            static size_t now() 
            {
                return (size_t)time(nullptr);
            }
        };
        class File
        {
        public:
            static bool exists(const std::string &pathname)
            {
                struct stat st;
                if (stat(pathname.c_str(), &st) < 0)
                    return false;
                return true;
            }
            static std::string path(const std::string &pathname)
            {
                //   a/b/c/d.txt
                size_t pos = pathname.find_last_of("/\\");
                if (pos == std::string::npos)
                    return ".";
                return pathname.substr(0, pos + 1);
            }
            static void create_directory(const std::string &pathname)
            {
                //  ./abc/def/ghi/a.txt
                if (pathname.empty() || exists(pathname)) return;
                size_t pos = 0, idx = 0;
                while (idx < pathname.size())
                {
                    pos = pathname.find_first_of("/\\", idx);
                    if (pos == std::string::npos)
                    {
                        mkdir(pathname.c_str(), 0777);
                        return;
                    }
                    std::string parent_dir = pathname.substr(0, pos + 1);
                    if (exists(parent_dir))
                    {
                        idx = pos + 1;
                        continue;
                    }
                    mkdir(parent_dir.c_str(), 0777);
                    idx = pos + 1;
                }
            }
        };
    }
}