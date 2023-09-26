/* 最简单的测试文件，单线程写入hello, world */
#include "zester/logging.h"
#include <string>
using namespace std;

int main()
{
    string loglevel = "DEBUG";
    string file_name = "test";
    string file_path = "../logs/";
    zest::Logger::InitGlobalLogger(loglevel, file_name, file_path, 1000000, 500, 25);
    LOG_DEBUG << "hello, world";

    return 0;
}