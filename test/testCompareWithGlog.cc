#include <glog/logging.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>

#include "Logger.h"
#include "AsyncLogger.h"

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"

xy::AsyncLogger g_asyncLogger("xy-AsyncLogger", 0x7fffffff);

void asyncOutput(const char* msg, int len)
{
    g_asyncLogger.append(msg, len);
}

void testSingleThread()
{
    char buf[128];
    for(int i = 0; i < 127; ++i)
    {
        buf[i] = 'a' + rand() % 26;
    }
    buf[127] = 0;

    std::cout << buf << std::endl;

    int cnt = 1000000;
    int k = 3;

    printf("glog start\n");
    for(int i = 0; i < k; ++i)
    {
        auto start1 = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < cnt; ++i)
        {
            LOG(INFO) << buf;
        }
        std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start1).count() << std::endl;
    }
    

    auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", "spdlog_async.log");
    async_file->set_level(spdlog::level::info);
    spdlog::set_default_logger(async_file);

    printf("spdlog start\n");
    for(int i = 0; i < k; ++i)
    {
        auto start3 = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < cnt; ++i)
        {
            spdlog::info("{}",buf);
        }
        std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start3).count() << std::endl;
    }
    
    g_asyncLogger.start();
    xy::Logger::setOutput(asyncOutput);

    printf("xylog start\n");
    for(int i = 0; i < k; ++i)
    {
        auto start2 = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < cnt; ++i)
        {
            LOG_INFO << buf;
        }
        std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start2).count() << std::endl;
    }
}

int main(int argc, char* argv[]) {

    FLAGS_log_dir = "./glog";
    // Initialize Google’s logging library.
    google::InitGoogleLogging("glog");


    testSingleThread();


    google::ShutdownGoogleLogging();

    return 0;
}