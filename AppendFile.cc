#include "AppendFile.h"

#include <assert.h>

using namespace xy;

AppendFile::AppendFile(std::string filename)
    :
    fp_(::fopen(filename.c_str(), "ae")),
    writtenBytes_(0)
{
    assert(fp_);
    // ::setbuffer(fp_, buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile()
{
    ::fclose(fp_);
}

void AppendFile::append(const char* logline, const size_t len)
{
    size_t written = 0;
    while(written != len)
    {
        size_t remain = len - written;
        size_t n = write(logline + written, remain);
        written += n;
    }
    writtenBytes_ += written;
}

void AppendFile::flush()
{
    ::fflush(fp_);
}

size_t AppendFile::write(const char* logline, size_t len)
{
    return ::fwrite_unlocked(logline, 1, len, fp_);
}