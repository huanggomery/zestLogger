/* 异步日志的对外接口 */
#ifndef ZEST_COMMON_LOGGING_H
#define ZEST_COMMON_LOGGING_H
#include "zest/common/async_logging.h"
#include "zest/common/config.h"
#include "zest/common/noncopyable.h"

namespace zest
{

class Logger: public noncopyable
{
    using self = Logger;
    using Buffer = FixedBuffer<SmallBufferSize>;
public:
    enum LogLevel
    {
        DEBUG = 0,
        INFO,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };

    // 供用户代码调用，初始化日志级别和AsyncLogger的配置，启动后端线程
    static void InitGlobalLogger();

    Logger() = delete;
    Logger(const std::string &basename, int line, LogLevel level);
    ~Logger();

public:
    self& operator<<(short);
    self& operator<<(unsigned short);
    self& operator<<(int);
    self& operator<<(unsigned int);
    self& operator<<(long);
    self& operator<<(unsigned long);
    self& operator<<(long long);
    self& operator<<(unsigned long long);

    self& operator<<(float v);
    self& operator<<(double v);

    self& operator<<(char v)
    {
        m_buffer.append(&v, 1);
        return *this;
    }

    self& operator<<(const char *str)
    {
        if (str)
            m_buffer.append(str, strlen(str));
        else
            m_buffer.append("(null)", 6);
        return *this;
    }

    self& operator<<(const std::string &v)
    {
        m_buffer.append(v.c_str(), v.size());
        return *this;
    }
    
private:
    void append(const char *data, int len) {m_buffer.append(data, len);}
    Buffer m_buffer;
};

extern Logger::LogLevel g_level;

#define LOG_DEBUG if (zest::g_level <= zest::Logger::DEBUG) \
    zest::Logger(__FILE__, __LINE__, zest::Logger::DEBUG)
#define LOG_INFO if (zest::g_level <= zest::Logger::INFO) \
    zest::Logger(__FILE__, __LINE__, zest::Logger::INFO)
#define LOG_ERROR if (zest::g_level <= zest::Logger::ERROR) \
    zest::Logger(__FILE__, __LINE__, zest::Logger::ERROR)
#define LOG_FATAL if (zest::g_level <= zest::Logger::FATAL) \
    zest::Logger(__FILE__, __LINE__, zest::Logger::FATAL)
    
} // namespace zest


#endif