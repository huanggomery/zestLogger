/* 异步日志的核心，定义了FixedBuffer和AsyncLogging */
#ifndef ZEST_COMMON_ASYNC_LOGGING_H
#define ZEST_COMMON_ASYNC_LOGGING_H
#include "zest/common/noncopyable.h"
#include "zest/common/sync.h"
#include <memory>
#include <vector>
#include <list>
#include <string.h>
#include <memory>

namespace zest
{
    
const int SmallBufferSize = 4000;           // 单条日志的最大长度
const int LargeBufferSize = 4000 * 1000;    // 每个前端缓冲区的大小
const int MaxNumericSize = 48;           // 数值型的最长长度


template <int SIZE>
class FixedBuffer: public noncopyable
{
public:
    FixedBuffer(): cur(m_data), m_lines(0){}
    ~FixedBuffer() = default;
    void bzero() {memset(m_data, 0, SIZE);}
    int avail() const {return SIZE-(cur-m_data);}
    int size() const {return cur-m_data;}
    int lines() const {return m_lines;}
    bool enough(int len) const {return avail() >= len;}
    void reset() {cur = m_data; m_lines = 0;}
    const char *data() const {return m_data;}
    void append(const char *logline, int len)
    {
        if (enough(len)) {
            memcpy(cur, logline, len);
            cur += len;
            ++m_lines;
        }
    }
    
private:
    char m_data[SIZE];
    char *cur;    // 尾后指针
    int m_lines;  // 记录条数
};


// 异步日志的核心类
class AsyncLogging: public noncopyable
{
    using SP_Self = std::shared_ptr<AsyncLogging>;
    using Buffer = FixedBuffer<LargeBufferSize>;
    using BufferPtr = std::unique_ptr<Buffer>;
    using BufferVector = std::vector<BufferPtr>;
    using BufferList = std::list<BufferPtr>;
public:
    static void InitAsyncLogger();   // 初始化函数，构造AsyncLogging对象，开启后端线程
    static SP_Self GetGlobalLogger();
    void append(const char *logline, int len);  // 供前端调用
    ~AsyncLogging();

private:
    static void *threadFunc(void *logger);  // 辅助调用后端线程
    void backendThreadFunc();   // 后端线程函数

    AsyncLogging(const std::string &file_name, const std::string &file_path, 
                 int max_file_size, int sync_interval, int max_buffers);

    // 计算当前缓冲区和刷盘缓冲区的距离，用于实时调整缓冲区数量
    int calcBufferDistance(BufferList::iterator begin, BufferList::iterator end);

private:
    /* 日志配置信息 */
    FILE *m_file;
    const std::string m_file_name;
    const std::string m_file_path;
    const int m_max_file_size;    // 单个日志文件中最大记录条数
    const int m_sync_interval;    // 日志刷盘间隔，单位ms
    const int m_max_buffers;      // 最多能拥有的buffer数

    int m_cur_file_size;          // 当前日志文件中记录条数

    /* 缓冲区信息 */
    BufferList m_buffers;      // 所有的缓冲区
    int m_buffers_n;           // 当前缓冲区数目
    BufferList::iterator m_current_itr;         // 当前正在写的缓冲区
    BufferList::iterator m_next_to_flush;       // 下一个需要刷盘的缓冲区

    /* 线程并发保护 */
    Mutex m_mutex;
    Condition m_cond;

    /* 后端线程的启停 */
    pthread_t m_tid;
    bool m_running;
};


} // namespace zest

#endif