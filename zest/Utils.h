// 一些小工具
#ifndef _UTILS_H
#define _UTILS_H
#include <string>

namespace zest
{
    
#define TESTOUT std::cout << "this is just a test" << std::endl;


// 返回当前时间的字符串,格式类似于： 20230706 21:05:57.229383
std::string get_time_str();

// 返回当前GMT时间字符串，格式类似于： Tue, 11 Jul 2023 07:22:04 GMT
std::string get_gmt_time_str();

// 生成日志文件名
std::string get_logfile_name(const std::string &file_name, const std::string &file_path);

// 检测文件夹是否存在
bool folderExists(const std::string& folderPath);

// 返回进程ID
pid_t getPid();

// 返回线程ID
pthread_t getTid();

} // namespace zest

#endif