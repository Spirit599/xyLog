#ifndef __XY_LOGFILE_H__
#define __XY_LOGFILE_H__

#include "AppendFile.h"

#include <mutex>
#include <memory>

namespace xy
{

class LogFile
{
public:
    LogFile(const std::string& name,
            off_t rollSize,
            bool threadSafe = true,
            int flushInterval = 1,
            int checkEveryN = 1024);
    
    ~LogFile() = default;

    void append(const char* logline, int len);
    void flush();
    bool rollFile();
private:

    void append_unlocked(const char* logline, int len);

    static std::string getLogFileName(const std::string& basename, time_t* now );

    const std::string basename_;
    const off_t rollSize_;
    const int flushInterval_;
    const int checkEveryN_;

    int count_;

    std::unique_ptr<std::mutex> mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    std::unique_ptr<AppendFile> file_;

    const static int kRollPerSeconds_ = 60 * 60 * 24;
};
}

#endif

