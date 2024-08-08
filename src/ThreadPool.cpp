#include "ThreadPool.h"
#include <assert.h>
#include <stdlib.h>

ThreadPool::ThreadPool(EventLoop* mainLoop, int count)
{
    this->m_index = 0;
    this->m_isStart = false;
    this->m_mainLoop = mainLoop;
    this->m_threadNum = count;
    this->m_workerThreads.clear();
}

ThreadPool::~ThreadPool()
{
    for (auto item : this->m_workerThreads)
    {
        delete item;
    }
}

void ThreadPool::run()
{
    assert(!this->m_isStart);
    if (this->m_mainLoop->getThreadID() != std::this_thread::get_id())
    {
        exit(0);
    }
    this->m_isStart = true;
    if (this->m_threadNum > 0)
    {
        for (int i = 0; i < this->m_threadNum; ++i)
        {
            WorkerThread* subThread = new WorkerThread(i);
            subThread->run();
            this->m_workerThreads.push_back(subThread);
        }
    }
}

EventLoop* ThreadPool::takeWorkerEventLoop()
{
    assert(this->m_isStart);
    if (this->m_mainLoop->getThreadID() != std::this_thread::get_id())
    {
        exit(0);
    }
    // 从线程池中找一个子线程, 然后取出里边的反应堆实例
    EventLoop* evLoop = this->m_mainLoop;
    if (m_threadNum > 0)
    {
        evLoop = this->m_workerThreads[this->m_index]->getEventLoop();
        this->m_index = ++this->m_index % this->m_threadNum;
    }
    return evLoop;
}
