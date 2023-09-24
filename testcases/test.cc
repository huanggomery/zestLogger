#include <iostream>
#include "zester/logging.h"
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <chrono>
using namespace std;

#define N 8
pthread_mutex_t g_mutex;

const int max_count = 1000000;

void *threadFunc(void *arg)
{
    int ms = 0;
    int *pi = static_cast<int *>(arg);
    int num = *pi;
    for (int i = 0; i < max_count; ++i) {
        LOG_DEBUG << "thread: " << num << " " << i;
        if (i % 1000 == 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        if (i % 100000 == 0)
            ms += 10;
    }
    return NULL;
}

int main()
{
    zest::Config::SetGlobalConfig("../conf/zest.xml");
    zest::Logger::InitGlobalLogger();
    pthread_t tid[N];
    int thread_nums[N];

    pthread_mutex_init(&g_mutex, NULL);

    for (int i = 0; i < N; ++i) {
        thread_nums[i] = i+1;
        pthread_create(tid+i, NULL, threadFunc, thread_nums+i);
    }

    for (int i = 0; i < N; ++i)
        pthread_join(tid[i], NULL);

    return 0;
}