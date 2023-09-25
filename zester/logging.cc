/* 异步日志的对外接口 */
#include "zester/logging.h"
#include "zester/util.h"
#include <unordered_map>
#include <algorithm>
#include <sys/stat.h>
#include <iostream>


namespace
{
std::unordered_map<std::string, zest::Logger::LogLevel> str2loglevel{
    {"DEBUG", zest::Logger::DEBUG},
    {"INFO", zest::Logger::INFO},
    {"ERROR", zest::Logger::ERROR},
    {"FATAL", zest::Logger::FATAL},
};

std::string loglevel2str[] = {
    "DEBUG",
    "INFO",
    "ERROR",
    "FATAL"
};


const char digits[] = "9876543210123456789";
const char *zero = digits + 9;

// 辅助整型变量写入到字符串中
template <typename Integer>
void formatInteger(zest::FixedBuffer<zest::SmallBufferSize> &buf, Integer value)
{
    if (buf.avail() < zest::MaxNumericSize)
        return;
    char ibuf[zest::MaxNumericSize];
    char *tmp = ibuf;
    Integer i = value;
    do
    {
        int lsd = i % 10;
        *tmp++ = zero[lsd];
        i /= 10;
    } while (i != 0);
    
    if (value < 0)
        *tmp++ = '-';
    *tmp = '\0';
    std::reverse(ibuf,tmp);
    buf.append(ibuf, tmp-ibuf);
}

} // namespace


namespace zest
{

// 全局的日志级别
Logger::LogLevel g_level;

// 供用户代码调用，初始化日志级别和AsyncLogger的配置，启动后端线程
void Logger::InitGlobalLogger(
        const std::string &loglevel, 
        const std::string &file_name, 
        const std::string &file_path, 
        int max_file_size, 
        int sync_interval, 
        int max_buffers
    )
{
    g_level = str2loglevel[loglevel];
    // 检查日志文件夹是否存在，不存在的话新建文件夹
    if (!folderExists(file_path)) {
        if (mkdir(file_path.c_str(), 0775) != 0) {
            std::cerr << "Create log file folder failed" << std::endl;
            exit(-1);
        }
    }
    AsyncLogging::InitAsyncLogger(file_name, file_path, max_file_size, sync_interval, max_buffers);
}

Logger::Logger(const std::string &basename, int line, LogLevel level)
{
    (*this) << loglevel2str[level] <<'\t' << get_time_str() << '\t'
            << getPid() << ':' << getTid() << '\t'
            << basename << ':' << line << '\t';
}

Logger::~Logger()
{
    (*this) << '\n';
    AsyncLogging::GetGlobalLogger()->append(m_buffer.data(), m_buffer.size());
}


/**************** 以下部分都是重载<<操作符 ****************/
Logger &Logger::operator<<(short v)
{
    (*this) << static_cast<int>(v);
    return *this;
}

Logger &Logger::operator<<(unsigned short v)
{
    (*this) << static_cast<unsigned int>(v);
    return *this;
}

Logger &Logger::operator<<(int v)
{
    formatInteger(m_buffer, v);
    return *this;
}

Logger &Logger::operator<<(unsigned int v)
{
    formatInteger(m_buffer, v);
    return *this;
}

Logger &Logger::operator<<(long v)
{
    formatInteger(m_buffer, v);
    return *this;
}

Logger &Logger::operator<<(unsigned long v)
{
    formatInteger(m_buffer, v);
    return *this;
}

Logger &Logger::operator<<(long long v)
{
    formatInteger(m_buffer, v);
    return *this;
}

Logger &Logger::operator<<(unsigned long long v)
{
    formatInteger(m_buffer, v);
    return *this;
}

Logger &Logger::operator<<(float v)
{
    (*this) << static_cast<double>(v);
    return *this;
}

Logger &Logger::operator<<(double v)
{
    if (m_buffer.avail() > MaxNumericSize)
    {
        char buf[MaxNumericSize] = {0};
        snprintf(buf, MaxNumericSize, "%.12g", v);
        m_buffer.append(buf, strlen(buf));
    }
    return *this;
}

/******************************************************/

} // namespace zest
