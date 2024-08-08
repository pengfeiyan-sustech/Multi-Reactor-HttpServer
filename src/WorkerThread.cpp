#include "WorkerThread.h"
#include <stdio.h>

// 子线程的回调函数
void WorkerThread::running()
{
    this->m_mutex.lock();
    this->m_evLoop = new EventLoop(m_name);
    this->m_mutex.unlock();
    this->m_cond.notify_one();
    this->m_evLoop->run();
}

WorkerThread::WorkerThread(int index)
{
    this->m_evLoop = nullptr;
    this->m_thread = nullptr;
    this->m_threadID = std::thread::id();
    this->m_name =  "SubThread-" + std::to_string(index);
}

WorkerThread::~WorkerThread()
{
    if (this->m_thread != nullptr)
    {
        delete this->m_thread;
    }
}

void WorkerThread::run()
{
    // 创建子线程
    this->m_thread = new std::thread(&WorkerThread::running, this);
    // 阻塞主线程, 让当前函数不会直接结束
    std::unique_lock<std::mutex> locker(m_mutex);
    while (this->m_evLoop == nullptr)
    {
        this->m_cond.wait(locker);
    }
}

// 如果子线程先执行，锁在子线程那里，如果子线程时间片用完，还没初始化好，
// 主线程加锁是加不上的，主线程时间片用完，子线程继续创建

// 如果主线程先执行，主线程加锁，发现evLoop为空，条件变量会把锁式释放掉，阻塞在这一步
// 等待子线程初始化好，条件变量发通知，主线程继续向下执行