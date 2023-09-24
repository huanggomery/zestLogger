/* 定义一些工具函数 */
#ifndef ZEST_COMMON_UTIL_H
#define ZEST_COMMON_UTIL_H
#include <string>

namespace zest
{

// 生成日志文件名
std::string get_logfile_name(const std::string &file_name, const std::string &file_path);

// 返回当前时间的字符串,格式类似于： 20230706 21:05:57.229383
std::string get_time_str();

// 返回进程ID
pid_t getPid();

// 返回线程ID
pthread_t getTid();

// 检测文件夹是否存在
bool folderExists(const std::string& folderPath);

} // namespace zest


#endif