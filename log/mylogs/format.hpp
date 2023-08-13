#pragma once
#include <memory>
#include "message.hpp"
#include "level.hpp"
#include "util.hpp"
#include <vector>
#include <cassert>
#include <sstream>
#include <utility>
namespace log
{
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os,const LogMsg& msg) = 0;
    };
    class MsgFormatItem: public FormatItem
    {
    public:
        virtual void format(std::ostream& os,const LogMsg& msg) override
        {
            os << msg._payload;
        }
    };
    class LevelFormatItem:public FormatItem
    {
    public:
        virtual void format(std::ostream& os,const LogMsg& msg)
        {
            os << LogLevel::toString(msg._level);
        }
    };
    class NameFormatItem:public FormatItem
    {
    public:
        virtual void format(std::ostream& os,const LogMsg& msg)
        {
            os << msg._name;
        }
    };
    class ThreadFormatItem:public FormatItem
    {
    public:
        void format(std::ostream& os,const LogMsg& msg)
        {
            os << msg._tid;
        }
    };
    class TimeFormatItem:public FormatItem
    {
    public:
        TimeFormatItem(const std::string& time_format = "%H:%M:%S") : _time_format(time_format)
        {
            if(time_format.empty()) _time_format = "%H:%M:%S";
        }
        virtual void format(std::ostream& os,const LogMsg& msg)
        {
            time_t t = msg._ctime;
            struct tm lt;
            localtime_r(&t,&lt);
            char tmp[128];
            strftime(tmp,sizeof(tmp)-1,_time_format.c_str(),&lt);
            os << tmp;
        }
    private:
        std::string _time_format;
    };
    class FileFormatItem:public FormatItem
    {
    public:
        virtual void format(std::ostream& os,const LogMsg& msg)
        {
            os << msg._file;
        }
    };
    class CLineFormatItem:public FormatItem
    {
    public:
        virtual void format(std::ostream& os,const LogMsg& msg)
        {
            os << msg._line;
        }
    };
    class TableFormatItem:public FormatItem
    {
    public:
        virtual void format(std::ostream& os,const LogMsg& msg)
        {
            os << "\t";
        }
    };
    class NLineFormatItem:public FormatItem
    {
    public:
        virtual void format(std::ostream& os,const LogMsg& msg)
        {
            os << "\n";
        }
    };
    class OtherFormatItem:public FormatItem
    {
    public:
        OtherFormatItem(const std::string &str = ""):_str(str){}
        virtual void format(std::ostream& os,const LogMsg& msg)
        {
            os << _str;
        }
    private:
        std::string _str;
    };
    /*
 %d ⽇期
 %T 缩进
 %t 线程id
 %p ⽇志级别
 %c ⽇志器名称
 %f ⽂件名
 %l ⾏号
 %m ⽇志消息
 %n 换⾏
 */
    class Formatter
    {
        public:
            using ptr = std::shared_ptr<Formatter>;
            Formatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n"):
            _pattern(pattern)
            {
                assert(parsePattern());
            }
            void format(std::ostream& out,LogMsg& msg)
            {
                for(auto& it : _items)
                {
                    it->format(out,msg);
                }
            }
            std::string format(LogMsg& msg)
            {
                std::stringstream ss;
                format(ss,msg);
                return ss.str();
            }
            private:
            //对格式化字符串进行解析
            bool parsePattern()
            {
                // ab%%cde[%d{%H%M%S}][%p]%T%m%n
                std::vector<std::pair<std::string,std::string>> fmt_order;
                size_t pos = 0;
                std::string key,val;
                while(pos < _pattern.size())
                {
                    //先处理正常的字符串
                    if(_pattern[pos] != '%')
                    {
                        val.push_back(_pattern[pos++]);
                        continue;
                    }
                    if(pos + 1 < _pattern.size() && _pattern[pos+1] == '%')
                    {
                        val.push_back('%');
                        pos += 2;
                        continue;
                    }
                    if (val.empty() == false)
                    {
                        fmt_order.push_back(std::make_pair("", val));
                        val.clear();
                    }

                    //往下走就是格式化字符的处理
                    pos += 1;
                    if(pos == _pattern.size())
                    {
                        std::cout << "%之后，没有对应的格式化字符" << std::endl;
                        return false;
                    }
                    key = _pattern[pos];
                    pos += 1;
                    if(pos < _pattern.size() && _pattern[pos] == '{')
                    {
                        pos += 1;
                        while(pos < _pattern.size() && _pattern[pos] != '}')
                        {
                            val.push_back(_pattern[pos++]);
                        }
                        if(pos == _pattern.size())
                        {
                            std::cout << "子规则“{}匹配出错”" << std::endl;
                            return false;
                        }
                        pos += 1;
                    }
                    fmt_order.push_back(std::make_pair(key,val));
                    key.clear();
                    val.clear();
                }
                for(auto &it : fmt_order)
                {
                    _items.push_back(createItem(it.first,it.second));
                }
                return true;
            }
            FormatItem::ptr createItem(const std::string& key,const std::string & val)
            {
                if(key == "d") return std::make_shared<TimeFormatItem>(val);
                if(key == "p") return std::make_shared<LevelFormatItem>();
                if(key == "t") return std::make_shared<ThreadFormatItem>();
                if(key == "c") return std::make_shared<NameFormatItem>();
                if(key == "f") return std::make_shared<FileFormatItem>();
                if(key == "l") return std::make_shared<CLineFormatItem>();
                if(key == "T") return std::make_shared<TableFormatItem>();
                if(key == "m") return std::make_shared<MsgFormatItem>();
                if(key == "n") return std::make_shared<NLineFormatItem>();
                return std::make_shared<OtherFormatItem>(val);
            }
        private:
            std::string _pattern;
            std::vector<FormatItem::ptr> _items;
    };
}