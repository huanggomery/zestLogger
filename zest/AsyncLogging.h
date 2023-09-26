// 异步日志

#ifndef _ASYNCLOGGING_H
#define _ASYNCLOGGING_H
#include <cstddef>
#include <cstring>
#include <stdio.h>
#include <memory>
#include <vector>
#include <pthread.h>
#include "zest/Sync.h"
#include "zest/noncopyable.h"
#include "zest/LogStream.h"


namespace zest
{
    
class AsyncLogging: public noncopyable
{
    using Buffer = FixedBuffer<LargeBufferSize>;
    using BufferPtr = std::unique_ptr<Buffer>;
    using BufferVector = std::vector<BufferPtr>;
public: 
    AsyncLogging(
        const std::string &file_name, 
        const std::string &file_path, 
        int flushIntervel);
    ~AsyncLogging();
    void append(const char *logline, int len);   // 前端函数
    void start();
    void stop();
    void threadFunc();  // 后端线程的函数

private:
    static std::vector<std::string> fileOpened_;  // 记录目前已经打开的文件

    FILE *file_;
    const std::string filename_;
    const std::string filepath_;
    const int flushInterval_;    // 刷盘间隔，单位ms
    bool running_;
    pthread_t tid_;
    Locker locker_;
    Conditon cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffer_;
};

} // namespace zest

#endif