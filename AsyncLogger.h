#ifndef __XY_ASYNC_LOGGER_H__
#define __XY_ASYNC_LOGGER_H__

#include "LogStream.h"
#include "Utils/CountDownLatch.h"

#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace xy
{

class AsyncLogger
{
public:
    AsyncLogger(const std::string& basename,
                off_t rollSize,
                int flushInterval = 1);

    ~AsyncLogger()
    {
        if(running_)
        {
            stop();
        }
    }

    void append(const char* logline, int len);

    void start()
    {
        running_ = true;
        thread_.reset(new std::thread(&AsyncLogger::threadFunc, this));
        latch_.wait();

    }

    void stop()
    {
        running_ = false;
        cv_.notify_all();
        thread_->join();
    }

private:

    void threadFunc();

    typedef xy::FixedBuffer<xy::kLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef std::unique_ptr<Buffer> BufferPtr;

    const int flushInterval_;
    bool running_;
    const std::string basename_;
    const off_t rollSize_;
    std::unique_ptr<std::thread> thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
    CountDownLatch latch_;
    
};

}

#endif