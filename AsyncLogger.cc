#include "AsyncLogger.h"
#include "LogFile.h"

#include <assert.h>
#include <chrono>

using namespace xy;

AsyncLogger::AsyncLogger(const std::string& basename,
                        off_t rollSize,
                        int flushInterval)
    :
    flushInterval_(flushInterval),
    running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_(nullptr),
    mutex_(),
    cv_(),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_(),
    latch_(1)
{
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

void AsyncLogger::append(const char* logline, int len)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if(currentBuffer_->avail() > len)
    {
        currentBuffer_->append(logline, len);
    }
    else
    {
        buffers_.push_back(std::move(currentBuffer_));
        if(nextBuffer_)
        {
            currentBuffer_ = std::move(nextBuffer_);
        }
        else
        {
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logline, len);
        cv_.notify_all();
    }
}

void AsyncLogger::threadFunc()
{
    assert(running_ == true);
    latch_.countDown();

    LogFile output(basename_, rollSize_, false);
    
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);

    unsigned long long sum = 0;

    while(running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());
        
        {
            std::unique_lock<std::mutex> ulk(mutex_);
            if(buffers_.empty())
            {
                cv_.wait_for(ulk, std::chrono::seconds(flushInterval_));
            }
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_)
            {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        for(const auto& buffer : buffersToWrite)
        {
            output.append(buffer->data(), buffer->length());
            sum += buffer->length();
        }

        if(buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);
        }

        if(!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if(!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }


    //dont lock
    buffers_.push_back(std::move(currentBuffer_));
    for(const auto& buffer : buffers_)
    {
        output.append(buffer->data(), buffer->length());
        sum += buffer->length();
    }
    output.flush();
}