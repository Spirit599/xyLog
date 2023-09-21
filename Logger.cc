#include "Logger.h"

#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>

namespace xy 
{

__thread char t_time[64];

Logger::LogLevel initLogLevel()
{
    if(::getenv("XY_LOG_TRACE"))
        return Logger::TRACE;
    else if(::getenv("XY_LOG_DEBUG"))
        return Logger::DEBUG;
    else
        return Logger::INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();

Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

class T
{
public:
    T(const char* str, unsigned len)
        :
        str_(str),
        len_(len)
    {}
    
    const char* str_;
    const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v)
{
    s.append(v.str_, v.len_);
    return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
    s.append(v.data_, v.size_);
    return s;
}

void defaultOutput(const char* msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);
  //FIXME check n
    (void)n;
}

void defaultFlush()
{
    fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

}

using namespace xy;

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
    :
    stream_(),
    level_(level),
    line_(line),
    filename_(file)
{
    formatTime();
    stream_ << T(LogLevelName[level], 6);
}

void Logger::Impl::formatTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);

    int mills = static_cast<int>(tv.tv_usec);

    int len = snprintf(t_time, sizeof(t_time), "[%4d-%02d-%02d %02d:%02d:%02d.%06d] ",
        tm.tm_year + 1900, 
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour, 
        tm.tm_min, 
        tm.tm_sec,
        mills);
    
    stream_ << T(t_time, len);
}

void Logger::Impl::finish()
{
    stream_ << " - " << filename_ << ':' << line_ << '\n';
}

Logger::Logger(SourceFile file, int line)
  : impl_(INFO, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
  : impl_(level, 0, file, line)
{
    impl_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
  : impl_(level, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, bool toAbort)
  : impl_(toAbort?FATAL:ERROR, errno, file, line)
{
}

Logger::~Logger()
{
    impl_.finish();
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if (impl_.level_ == FATAL)
    {
        g_flush();
        abort();
    }
}

void Logger::setLogLevel(Logger::LogLevel level)
{
    g_logLevel = level;
}

void Logger::setOutput(OutputFunc out)
{
    g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
    g_flush = flush;
}