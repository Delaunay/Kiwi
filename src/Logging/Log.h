#pragma once

#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include <algorithm>
#include <regex>

#include "../Format.h"
#include "../Types.h"

namespace kiwi {

#define BIG_SCREEN " "
#define SMALL_SCREEN "\n\t"
#define FILE_SEPARATOR BIG_SCREEN
#define MSG_SEPARATOR " "
#define SIMPLIFY_NAMES simplify_function_name

#define KIWI_LOG_LEVEL                                                                             \
    KIWI_LEVEL(all, All, "     ALL     ")                                                          \
    KIWI_LEVEL(trace, Trace, "     TRACE   ")                                                      \
    KIWI_LEVEL(debug, Debug, " [?] DEBUG   ")                                                      \
    KIWI_LEVEL(warn, Warning, " /!\\ WARNING ")                                                    \
    KIWI_LEVEL(info, Info, " [i] INFO    ")                                                        \
    KIWI_LEVEL(error, Error, " [!] ERROR   ")

enum class LogLevel {
#define KIWI_LEVEL(a, b, c) a,
    KIWI_LOG_LEVEL
#undef KIWI_LEVEL
};

class Log {
  public:
    template <typename... Args>
    void log(std::string file, std::string function, i64 line, LogLevel level,
             const Args &... args) {
        if(log_level <= level)
            print(out, date(), get_string(level), pretty_file(file), FILE_SEPARATOR,
                  SIMPLIFY_NAMES(function), " ", line, MSG_SEPARATOR, args...)
                << std::endl;
    }

    template <typename... Args>
    void log_call_trace(std::string file, std::string function, i64 line, LogLevel level, i64 depth,
                        const Args &... args) {
        if(log_level <= level)
            print(out, get_string(level), pretty_depth(depth), pretty_file(file), " ",
                  SIMPLIFY_NAMES(function), ":", line, " ", args...)
                << std::endl;
    }

    // replace something like `kiwi::Class::method` by `k.C.method`
    std::string simplify_function_name(const std::string &function) {
        // Match namespace like name `kiwi::Class::method`
        std::regex e("([A-Za-z])[A-Za-z0-9]*::");

        std::string result;
        std::regex_replace(std::back_inserter(result), function.begin(), function.end(), e, "$1.");

        return result;
    }

    static std::string date() {
        static std::string date_buffer = std::string(20, ' ');

        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(&date_buffer[0], 20, "%Y-%m-%d %H:%M:%S", timeinfo);
        return date_buffer.substr(0, 19);
    }

    static const std::string &get_string(LogLevel level) {
        static std::vector<std::string> stuff = {
#define KIWI_LEVEL(a, b, c) c,
            KIWI_LOG_LEVEL
#undef KIWI_LEVEL
        };

        return stuff[unsigned(level)];
    }

    static std::string pretty_file(const std::string &file_name) {
        int count = 0;
        std::string temp;
        std::vector<std::string> strings;

        for(auto c : file_name) {
            if(c == '/' || c == '\\') {
                count += 1;
                strings.push_back(temp);
                temp = "";
            } else {
                temp += c;
            }
        }
        strings.push_back(temp);

        std::size_t size = strings.size();
        if(size > 2)
            return strings[size - 2] + "/" + strings[size - 1];

        return strings[size - 1];
    }

    /* +->          i = 0
     * |+->         i = 1
     * |:+->        i = 2
     * |:|+->       i = 3
     * |:+->
     * |+->
     * +->
     */
    static std::string pretty_depth(u64 d) {
        std::string s(d + 4, ' ');

        for(u64 i = 0; i < d; ++i) {
            if(i % 2 == 0)
                s[i] = '|';
            else
                s[i] = ':';
        }
        s[d + 0] = '+';
        s[d + 1] = '-';
        s[d + 2] = '>';
        return s;
    }

    void set_log_level(LogLevel level) { log_level = level; }

  private:
    // std::vector<LogEntry> entries;
    std::ostream &out;
    LogLevel log_level = LogLevel::all;

  public:
    static std::string header() {
        return "-----------------------------------------------------------------------------\n"
               "YYYY-MM-DD HH:MM:SS |  SEVERITY | FILE:FUNCTION:LINE                  MESSAGE\n"
               "--------------------+-----------+--------------------------------------------\n";
    }

    static Log &instance() {
        static Log log(header());
        return log;
    }

  private:
    Log(const std::string &header) : out(std::cout) { out << header; }
};

#ifndef __linux__
#define GET_FUN_NAME_EXT __FUNCSIG__
#else
#define GET_FUN_NAME_EXT __PRETTY_FUNCTION__
#endif

#define GET_FUN_NAME_SHORT __FUNCTION__

#define GET_FUN_NAME GET_FUN_NAME_SHORT

#define LOG_INTERNAL(...)                                                                   \
    kiwi::Log::instance().log(__FILE__, GET_FUN_NAME, __LINE__, __VA_ARGS__)

#define LOG_INTERNAL_CALL(...)                                                       \
    kiwi::Log::instance().log_call_trace(__FILE__, GET_FUN_NAME, __LINE__, __VA_ARGS__)

#define LWARN kiwi::LogLevel::warn
#define LINFO kiwi::LogLevel::info
#define LERROR kiwi::LogLevel::error
#define LDEBUG kiwi::LogLevel::debug
#define LTRACE kiwi::LogLevel::trace

#define LOG(...) LOG_INTERNAL(__VA_ARGS__)
#define TRACE(...) LOG_INTERNAL_CALL(__VA_ARGS__)

#define log_warn(...) LOG(LWARN, __VA_ARGS__)
#define log_info(...) LOG(LINFO, __VA_ARGS__)
#define log_error(...) LOG(LERROR, __VA_ARGS__)
#define log_debug(...) LOG(LDEBUG, __VA_ARGS__)
#define log_trace(...) LOG(LTRACE, __VA_ARGS__)

#define log_cwarn(...) TRACE(LWARN, __VA_ARGS__)
#define log_cinfo(...) TRACE(LINFO, __VA_ARGS__)
#define log_cerror(...) TRACE(LERROR, __VA_ARGS__)
#define log_cdebug(...) TRACE(LDEBUG, __VA_ARGS__)
#define log_ctrace(...) TRACE(LTRACE, __VA_ARGS__)

#define debug_if(pred, ...)                                                                        \
    {                                                                                              \
        if(pred) {                                                                                 \
            log_debug(__VA_ARGS__);                                                                \
        }                                                                                          \
    }
}

/*
template<typename T, typename... Args>
std::string printf(const char *s, T value, Args... args)
{
    bool escaped = false;

    while (*s) {

        if (*s == '\\'){
            escaped = true;
            s += 1;
        }
        else if (*s == '%' && !escaped) {
            switch (*(s + 1)){
            case 'f':{
                out << value;
                s += 2;
                return printf(s, args...);
            }
            case 'i':{
                out << value;
                s += 2;
                return printf(s, args...);
            }
            default:{
                out << value;
                s += 2;
                printf(s, args...);
            }
            }
        }
        else{
            s += 1;
            out << *s;
            escaped = false;
        }
    }
}*/
/*
    struct LogEntry{
        LogEntry(LogLevel level, std::string datetime, std::string file,
                 std::string function, int line, std::string message, int depth):
            level(level), datetime(datetime), file(file), function(function),
            line(line), message(message), depth(depth)
        {
            if (max_depth() < depth)
                max_depth() = depth;
        }

        LogLevel    level;
        std::string datetime;
        std::string file;
        std::string function;
        int         line;
        std::string message;
        int         depth;

        static int&  max_depth(){
            static int depth = 0;
            return depth;
        }

        void print(std::ostream& out) const{
            if (depth < 0)
                out << "| "  << datetime << " |"
                    << get_string(level) << "| "
                    << file              << ":"
                    << line              << " | ";
            else
                out << pretty_depth(unsigned(depth));

            out << function << " | "
                << message  << "\n";
        }
    };*/
