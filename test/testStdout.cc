#include "Logger.h"
#include "AsyncLogger.h"

xy::AsyncLogger g_asyncLogger("xy-AsyncLogger", 4096);

void asyncOutput(const char* msg, int len)
{
    g_asyncLogger.append(msg, len);
}

void testAsync()
{
    xy::Logger::setOutput(asyncOutput);
    LOG_INFO << "testAsync";
    struct timespec ts = { 0, 500*1000*1000 };
    nanosleep(&ts, NULL);
}


int main(int argc, char const *argv[])
{
    // xy::AsyncLogger g_asyncLogger("xy-AsyncLogger");
    g_asyncLogger.start();
    testAsync();

    return 0;
}
