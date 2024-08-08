#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include "EventLoop.h"


/**
 * @brief 工作线程类
 */
class WorkerThread
{
public:
    /**
     * @brief 构造函数
     */
    WorkerThread(int index);
    /**
     * @brief 析构函数
     */
    ~WorkerThread();
    /**
     * @brief 创建子线程
     */
    void run();
    /**
     * @brief 取出一个反应堆，适用于主线程
     */
    inline EventLoop* getEventLoop()
    {
        return m_evLoop;
    }

private:
    /**
     * @brief 子线程回调函数
     */
    void running();

private:
    std::thread* m_thread;   // 保存线程的实例
    std::thread::id m_threadID; // ID
    std::string m_name;
    std::mutex m_mutex;  // 互斥锁
    std::condition_variable m_cond;    // 条件变量
    EventLoop* m_evLoop;   // 反应堆模型
};


#endif