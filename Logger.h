#ifndef __XY_LOGGER_H__
#define __XY_LOGGER_H__


#include "LogStream.h"

#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <mutex>

namespace xy
{

//日志输出器
class Logger {
public:
    enum LogLevel : uint8_t
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    class SourceFile
    {
    public:
        template<int N>
        SourceFile(const char (&arr)[N])
            :
            data_(arr),
            size_(N-1)
        {
            const char* slash = strrchr(data_, '/');
            if(slash)
            {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char* filename)
            :
            data_(filename)
        {
            const char* slash = strrchr(filename, '/');
            if(slash)
            {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream()
    { return impl_.stream_;}

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char* msg, int len);
    typedef void (*FlushFunc)();
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

private:
    class Impl
    {
    public:
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
        void finish();
        void formatTime();

        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile filename_;
    };

    Impl impl_;
};

#define LOG_TRACE if (xy::Logger::logLevel() <= xy::Logger::TRACE) \
  xy::Logger(__FILE__, __LINE__, xy::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (xy::Logger::logLevel() <= xy::Logger::DEBUG) \
  xy::Logger(__FILE__, __LINE__, xy::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (xy::Logger::logLevel() <= xy::Logger::INFO) \
  xy::Logger(__FILE__, __LINE__, xy::Logger::INFO, __func__).stream()
#define LOG_WARN xy::Logger(__FILE__, __LINE__, xy::Logger::WARN, __func__).stream()
#define LOG_ERROR xy::Logger(__FILE__, __LINE__, xy::Logger::ERROR, __func__).stream()
#define LOG_FATAL xy::Logger(__FILE__, __LINE__, xy::Logger::FATAL, __func__).stream()
#define LOG_SYSERR xy::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL xy::Logger(__FILE__, __LINE__, true).stream()

}

#endif