#include "zest/Logging.h"
#include "zest/Utils.h"
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <sys/stat.h>
#include <iostream>

namespace {
pthread_once_t once = PTHREAD_ONCE_INIT;
std::shared_ptr<zest::AsyncLogging> AsyncLogger_(nullptr);
std::string LevelStr[] = {
    "DEBUG",
    "INFO",
    "ERROR",
    "FATAL"
};
std::unordered_map<std::string, zest::Logger::LogLevel> str2loglevel{
    {"DEBUG", zest::Logger::DEBUG},
    {"INFO", zest::Logger::INFO},
    {"ERROR", zest::Logger::ERROR},
    {"FATAL", zest::Logger::FATAL}
};

} // namespace


namespace zest
{
    
// 全局的日志级别
Logger::LogLevel Logger::g_level_(INFO);

void Logger::InitGlobalLogger(
        const std::string &loglevel, 
        const std::string &file_name, 
        const std::string &file_path, 
        int flushIntervel)
{
    if (!folderExists(file_path)) {
        if (mkdir(file_path.c_str(), 0775) != 0) {
            std::cerr << "Create log file folder failed" << std::endl;
            exit(-1);
        }
    }
    g_level_ = str2loglevel[loglevel];
    AsyncLogger_.reset(new AsyncLogging(file_name, file_path, flushIntervel));
    AsyncLogger_->start();
}


// 往异步日志的前端写
void output(const char *logline, int len)
{
    AsyncLogger_->append(logline, len);
}

Logger::Logger(const std::string &basename, int line, LogLevel level):
    impl_(basename, line, level)
{

}

Logger::~Logger()
{

}

Logger::Impl::Impl(const std::string &basename, int line, LogLevel level):
    stream_(), basename_(basename), line_(line), level_(level)
{
    stream_ << LevelStr[level] << '\t' << get_time_str() << '\t'
            << getPid() << ':' << getTid() << '\t'
            << basename << ':' << line << '\t';
}

Logger::Impl::~Impl()
{
    stream_ << '\n';
    output(stream_.buffer().getData(), stream_.buffer().size());
}

} // namespace zest
