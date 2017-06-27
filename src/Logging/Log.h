#pragma once

#include <vector>
#include <iostream>
#include <chrono>
#include <cstdio>
#include <string>
#include <algorithm>

namespace kiwi{

#define KIWI_LOG_LEVEL \
    KIWI_LEVEL(all, All, "     ALL     ")\
    KIWI_LEVEL(trace, Trace, "     TRACE   ")\
    KIWI_LEVEL(debug, Debug, " [?] DEBUG   ")\
    KIWI_LEVEL(warning, Warning, " /!\\ WARNING ")\
    KIWI_LEVEL(info, Info, " [i] INFO    ")\
    KIWI_LEVEL(error, Error, " [!] ERROR   ")\
    KIWI_LEVEL(call_trace, CallTrace, "")


enum class LogLevel{
#define KIWI_LEVEL(a, b, c) a,
    KIWI_LOG_LEVEL
#undef KIWI_LEVEL
};

class Log{
public:

    void log(LogLevel level, std::string datetime, std::string file,
             std::string function, int line, std::string message, int depth)
    {
        if (log_level <= level)
            LogEntry(level, datetime,  pretty_file(file), function, line, message, depth)
                    .print(out);
    }

    void flush(){
        for(const auto& entry: entries){
            if (log_level <= entry.level)
                entry.print(out);
        }
        entries.clear();
    }

    static std::string date(){
        static std::string date_buffer = std::string(20, ' ');

        time_t rawtime;
        struct tm* timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(&date_buffer[0], 20, "%Y-%m-%d %H:%M:%S", timeinfo);
        return date_buffer;
   }

    static const std::string& get_string(LogLevel level){
        static std::vector<std::string> stuff = {
        #define KIWI_LEVEL(a, b, c) c,
            KIWI_LOG_LEVEL
        #undef KIWI_LEVEL
        };

        return stuff[unsigned(level)];
    }

    static std::string pretty_file(const std::string& file_name){
        int count = 0;
        std::string temp;
        std::vector<std::string> strings;

        for(auto c : file_name){
            if (c == '/' || c == '\\'){
                count += 1;
                strings.push_back(temp);
                temp = "";
            } else {
                temp += c;
            }
        }
        strings.push_back(temp);

        std::size_t size = strings.size();
        if (size > 2)
            return strings[size - 2] + "/" + strings[size - 1];

        return strings[size - 1];
    }

public:
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
                out << "| "  << datetime << "|"
                    << get_string(level) << "| "
                    << file              << ":"
                    << line              << " | ";
            else
                out << pretty_depth(unsigned(depth));

            out << function << " | "
                << message  << "\n";
        }
    };

    static std::string pretty_depth(unsigned int d){
        std::string s(d + 4, ' ');

        for(unsigned int i = 0; i < d; ++i){
            if (i % 2 == 0)
                s[i] = '|';
            else
                s[i] = ':';
        }
        s[d + 0] = '+';
        s[d + 1] = '-';
        s[d + 2] = '>';
        return s;
    }


    /* +->          i = 0
     * |+->         i = 1
     * |:+->        i = 2
     * |:|+->       i = 3
     * |:|:+->      i = 4
     * |:|:|+->     i = 5
     * |:|:|:+->    i = 6
     * |:|:|:|+->   i = 7
     * |:|:|:+->
     * |:|:|+->
     * |:|:+->
     * |:|+->
     * |:+->
     * |+->
     * +->
     */
private:
    std::vector<LogEntry> entries;
    std::ostream&         out;
    LogLevel              log_level = LogLevel::warning;

public:

    static std::string header(){
        return
        ".-----------------------------------------------------------------------------.\n"
        "| YYYY-MM-DD HH:MM:SS |  SEVERITY   | FILE:LINE | FUNCTION | MESSAGE          |\n"
        "|---------------------+-------------+-----------------------------------------|\n";
    }

    static Log& instance(){
        static Log log(header());
        return log;
    }
#undef debug
    static void warning(std::string file, std::string function, int line, std::string message, int depth = -1){
        Log::instance().log(LogLevel::warning, date(), file, function, line, message, depth);
    }

    static void info(std::string file, std::string function, int line, std::string message, int depth = -1){
        Log::instance().log(LogLevel::info, date(), file, function, line, message, depth);
    }

    static void error(std::string file, std::string function, int line, std::string message, int depth = -1){
        Log::instance().log(LogLevel::error, date(), file, function, line, message, depth);
    }

    static void debug(std::string file, std::string function, int line, std::string message, int depth = -1){
        Log::instance().log(LogLevel::debug, date(), file, function, line, message, depth);
    }

    static void trace(std::string file, std::string function, int line, std::string message, int depth = -1){
        Log::instance().log(LogLevel::trace, date(), file, function, line, message, depth);
    }

    static void call_trace(std::string file, std::string function, int line, std::string message, int depth = -1){
        Log::instance().log(LogLevel::call_trace, date(), file, function, line, message, depth);
    }

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
    }

private:
    Log(const std::string& header):
        out(std::cout)
    {
        out << header;
    }
};

#define log_warn(message)\
    kiwi::Log::warning(__FILE__, __func__, __LINE__, message)

#define log_info(message)\
    kiwi::Log::info   (__FILE__, __func__, __LINE__, message)

#define log_error(message)\
    kiwi::Log::error  (__FILE__, __func__, __LINE__, message)

#define log_debug(message)\
    kiwi::Log::debug  (__FILE__, __func__, __LINE__, message)

#define log_trace(message)\
    kiwi::Log::trace  (__FILE__, __func__, __LINE__, message)

#define log_rec(depth, message)\
    kiwi::Log::call_trace(__FILE__, __func__, __LINE__, message, depth)
}
