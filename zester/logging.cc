/* 异步日志的对外接口 */
#include "zest/common/logging.h"
#include "zest/common/util.h"
#include <unordered_map>
#include <algorithm>

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
void Logger::InitGlobalLogger()
{
    Config *cfg = Config::GetGlobalConfig();
    g_level = str2loglevel[cfg->log_level()];
    AsyncLogging::InitAsyncLogger();
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
