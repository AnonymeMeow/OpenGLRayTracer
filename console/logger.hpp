#pragma once

#include <iostream>
#include <format>

class Logger
{
    inline static const bool stdio_sync = std::ios::sync_with_stdio(false);
    const std::string module_name;
    void print(std::ostream&, const std::string&, const std::string&) const;
public:
    enum LogLevel
    {
        MUTE,
        ERROR,
        INFO,
        TRACE
    };
    Logger(const std::string&);
    inline static LogLevel log_level = INFO;

    template <typename... Args>
    void error(std::format_string<Args...> fmt_str, Args&&... args) const
    {
        if (log_level >= ERROR)
        {
            print(std::cerr, "\033[1m\033[31m[ERROR]", std::format(fmt_str, std::forward<Args>(args)...));
        }
    }
    
    template <typename... Args>
    void info(std::format_string<Args...> fmt_str, Args&&... args) const
    {
        if (log_level >= INFO)
        {
            print(std::cout, "[INFO]", std::format(fmt_str, std::forward<Args>(args)...));
        }
    }

    template <typename... Args>
    void trace(std::format_string<Args...> fmt_str, Args&&... args) const
    {
        if (log_level >= TRACE)
        {
            print(std::cout, "\033[2m\033[30m[TRACE]", std::format(fmt_str, std::forward<Args>(args)...));
        }
    }
};