// 日志系统的外部接口
#ifndef _LOGGING_H
#define _LOGGING_H
#include "zest/LogStream.h"
#include "zest/AsyncLogging.h"
#include "zest/noncopyable.h"


namespace zest
{
    
class Logger: public noncopyable
{
public:
    enum LogLevel
    {
        DEBUG=0,
        INFO,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    // 供用户代码调用，初始化日志级别和AsyncLogger的配置，启动后端线程
    static void InitGlobalLogger(
        const std::string &loglevel, 
        const std::string &file_name, 
        const std::string &file_path, 
        int flushIntervel
    );

    Logger(const std::string &basename, int line, LogLevel level = INFO);
    ~Logger();
    LogStream &stream() {return impl_.stream_;}
    static LogLevel level() {return g_level_;}

private:
    struct Impl
    {
        Impl(const std::string &basename, int line, LogLevel level);
        ~Impl();

        LogStream stream_;
        const std::string basename_;
        int line_;
        LogLevel level_;
    };

    static LogLevel g_level_;
    Impl impl_;
};

#define LOG_DEBUG if (zest::Logger::level() <= zest::Logger::DEBUG) \
    zest::Logger(__FILE__, __LINE__, zest::Logger::DEBUG).stream()
#define LOG_INFO if (zest::Logger::level() <= zest::Logger::INFO) \
    zest::Logger(__FILE__, __LINE__).stream()
#define LOG_ERROR if (zest::Logger::level() <= zest::Logger::ERROR) \
    zest::Logger(__FILE__, __LINE__, zest::Logger::ERROR).stream()
#define LOG_FATAL if (zest::Logger::level() <= zest::Logger::FATAL) \
    zest::Logger(__FILE__, __LINE__, zest::Logger::FATAL).stream()

} // namespace zest

#endif