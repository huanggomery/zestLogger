/* 还有点BUG，最后会漏掉几条记录没有写进日志文件 */
#include "zest/AsyncLogging.h"
#include "zest/Utils.h"
#include <algorithm>
#include <unistd.h>

namespace {
    void *threadFunction(void *arg)
    {
        zest::AsyncLogging *AsyncLogger_ = reinterpret_cast<zest::AsyncLogging*>(arg);
        AsyncLogger_->threadFunc();
        return NULL;
    }
} // namespace


namespace zest
{
    
std::vector<std::string> AsyncLogging::fileOpened_;

AsyncLogging::AsyncLogging(
        const std::string &file_name, 
        const std::string &file_path, 
        int flushIntervel):
    file_(fopen(get_logfile_name(file_name, file_path).c_str(), "a")),
    filename_(file_name),
    filepath_(file_path),
    flushInterval_(flushIntervel),
    running_(false),
    locker_(),
    cond_(locker_),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer)
{
    if (std::find(fileOpened_.begin(), fileOpened_.end(), file_name) != fileOpened_.end())
        throw std::runtime_error("Filename has been opened");

    fileOpened_.push_back(file_name);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffer_.reserve(16);
}

AsyncLogging::~AsyncLogging()
{
    if (running_)
        stop();
}

void AsyncLogging::start() 
{
    running_ = true;
    pthread_create(&tid_, NULL, threadFunction, this);
}

void AsyncLogging::stop()
{
    running_ = false;
    cond_.signal();
    pthread_join(tid_, NULL);
}

void AsyncLogging::append(const char *logline, int len)
{
    locker_.lock();
    if (currentBuffer_->avail() >= len)
    {
        currentBuffer_->append(logline, len);  // 最多的情况
    }
    else
    {
        buffer_.push_back(std::move(currentBuffer_));
        if (nextBuffer_)
            currentBuffer_.reset(nextBuffer_.release());   // 较少发生
        else
            currentBuffer_.reset(new Buffer);    // 很少发生
        currentBuffer_->append(logline, len);
        cond_.signal();
    }

    locker_.unlock();
}

void AsyncLogging::threadFunc()
{
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);

    do
    {
        locker_.lock();

        if (buffer_.empty())
            cond_.waitForMilliSeconds(flushInterval_);
        buffer_.push_back(std::move(currentBuffer_));
        currentBuffer_.reset(newBuffer1.release());
        buffersToWrite.swap(buffer_);
        if (!nextBuffer_)
            nextBuffer_.reset(newBuffer2.release());

        locker_.unlock();

        char errorBuf[256] = {0};
        // 丢弃过多的数据
        if (buffersToWrite.size() > 25)
        {
            snprintf(errorBuf, sizeof(errorBuf), "Dropped log messages at %s, %zd larger buffers\n",
                     get_time_str().c_str(), buffersToWrite.size()-2);
            
            buffersToWrite.erase(buffersToWrite.begin() +2, buffersToWrite.end());
        }

        // 写入硬盘
        for (auto &bp : buffersToWrite)
        {
            fwrite(bp->getData(), 1, bp->size(), file_);
        }
        if (strlen(errorBuf) > 0)
            fwrite(errorBuf, 1, strlen(errorBuf), file_);
        
        // 重新填充newBuffer1和newBuffer2
        if (!newBuffer1)
        {
            newBuffer1.reset(buffersToWrite.back().release());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }
        if (!newBuffer2)
        {
            newBuffer2.reset(buffersToWrite.back().release());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        buffersToWrite.clear();

        // flush
        fflush(file_);

        /* 人为地制造延时，模拟IO较慢的情况，最终编译时必须注释掉，否则将影响性能！*/
        // sleep(1);
        /***************************************************************/
    } while (running_);

    /* 将剩余的内容刷盘 */
    locker_.lock();
    buffer_.push_back(std::move(currentBuffer_));
    for (auto &bp : buffer_) {
        fwrite(bp->getData(), 1, bp->size(), file_);
    }
    locker_.unlock();

    fflush(file_);
    fclose(file_);
    auto it = std::find(fileOpened_.begin(), fileOpened_.end(), filename_);
    if (it != fileOpened_.end())
        fileOpened_.erase(it);
}

} // namespace zest

