#include "zest/Utils.h"
#include <sys/time.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syscall.h>

namespace zest
{

static pid_t g_pid = 0;
static thread_local pthread_t t_tid = 0;
    
// 返回当前时间的字符串,格式类似于： 20230706 21:05:57.229383
std::string get_time_str()
{
    char str_t[27] = {0};
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t sec = tv.tv_sec;
    struct tm *ptime = localtime(&sec);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S", ptime);

    char usec[8];
    snprintf(usec, sizeof(usec), ".%ld", tv.tv_usec);
    strcat(str_t, usec);

    return std::string(str_t);
}

// 返回当前GMT时间字符串，格式类似于： Tue, 11 Jul 2023 07:22:04 GMT
std::string get_gmt_time_str()
{
    time_t currentTime = time(NULL);
    struct tm *gmTime = gmtime(&currentTime);
    char timeString[50];

    strftime(timeString, sizeof(timeString), "%a, %d %b %Y %H:%M:%S GMT", gmTime);
    return std::string(timeString);
}

// 生成日志文件名
std::string get_logfile_name(const std::string &file_name, const std::string &file_path)
{
    char str_t[25] = {0};
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t sec = tv.tv_sec;
    struct tm *ptime = localtime(&sec);
    strftime(str_t, 26, "%Y%m%d%H%M%S", ptime);

    return std::string(file_path + file_name + str_t + ".log");
}

// 检测文件夹是否存在
bool folderExists(const std::string& folderPath)
{
    struct stat info;

    if (stat(folderPath.c_str(), &info) != 0) {
        return false; // stat 函数失败，文件夹不存在
    }

    return S_ISDIR(info.st_mode);
}

// 返回进程ID
pid_t getPid()
{
    return g_pid != 0 ? g_pid : (g_pid = getpid());
}

// 返回线程ID
pthread_t getTid()
{
    return t_tid != 0 ? t_tid : (t_tid = syscall(SYS_gettid));
}

} // namespace zest
