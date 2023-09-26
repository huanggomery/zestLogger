/* 使用多线程，向文件写入连续的数字 */
#include <iostream>
#include "zester/logging.h"
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <chrono>
using namespace std;

int global_number = 1;

const int N = 5;
const int max_count = 500000;
const int delay = 100;

struct Task
{
    pthread_t tid;
    int num;         // 线程编号
    int max_count;   // 写入数量
    int delay;       // 延迟系数，控制每10000条记录后延迟多少ms
};


void *threadFunc(void *arg)
{
    Task *task = static_cast<Task *>(arg);
    int num = task->num;
    for (int i = 0; i < task->max_count; ++i) {
        LOG_DEBUG << "thread: " << num << " " << i;
        if (i % 10000 == 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(task->delay));
    }
    return NULL;
}

void MultiThreadsWrite(int n, int max_count, int delay)
{
    Task *tasks = new Task[n];
    for (int i = 0; i < n; ++i) {
        tasks[i].num = global_number++;
        tasks[i].max_count = max_count;
        tasks[i].delay = delay;
        pthread_create(&tasks[i].tid, NULL, threadFunc, tasks+i);
    }

    for (int i = 0; i < n; ++i)
        pthread_join(tasks[i].tid, NULL);

    delete []tasks;
}

int main()
{
    zest::Logger::InitGlobalLogger("DEBUG", "test", "../logs/", 5000000, 500, 25);
    
    // 热身
    MultiThreadsWrite(5, 10000, 0);

    // 计算时间部分
    auto start = std::chrono::high_resolution_clock::now();

    // 调用该函数
    MultiThreadsWrite(N, max_count, delay);

    // 计算时间部分
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << "spend time: " << duration << "ms" << endl;

    return 0;
}