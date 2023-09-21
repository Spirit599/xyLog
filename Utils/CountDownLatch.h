#ifndef __XY_COUNTDOWNLATCH_H__
#define __XY_COUNTDOWNLATCH_H__

#include <assert.h>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace xy
{
class CountDownLatch
{
public:
    CountDownLatch(int cnt) : cnt_(cnt)
    {
        assert(cnt_ > 0);
    }

    void wait()
    {
        std::unique_lock<std::mutex> ulk(mutex_);
        while(cnt_ != 0)
        {
            cv_.wait(ulk);
        }
    }

    void countDown()
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        --cnt_;
        if(cnt_ == 0)
        {
            cv_.notify_all();
        }
    }

private:
    int cnt_;
    std::mutex mutex_;
    std::condition_variable cv_;
    
};
}

#endif