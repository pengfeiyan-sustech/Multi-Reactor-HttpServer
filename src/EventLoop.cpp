#include "EventLoop.h"
#include <assert.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SelectDispatcher.h"
#include "PollDispatcher.h"
#include "EpollDispatcher.h"

EventLoop::EventLoop() : EventLoop(std::string())
{
}

EventLoop::EventLoop(const std::string threadName)
{
    this->m_isQuit = true;    // 默认没有启动
    this->m_threadID = std::this_thread::get_id();
    this->m_threadName = threadName == std::string() ? "MainThread" : threadName;
    this->m_dispatcher = new SelectDispatcher(this);
    // map
    this->m_channelMap.clear();
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, this->m_socketPair);
    if (ret == -1)
    {
        perror("socketpair");
        exit(0);
    }
#if 0
    // 指定规则: evLoop->socketPair[0] 发送数据, evLoop->socketPair[1] 接收数据
    Channel* channel = new Channel(this->m_socketPair[1], FDEvent::ReadEvent,
        readLocalMessage, nullptr, nullptr, this);
#else
    // 绑定 - bind
    auto obj = std::bind(&EventLoop::readMessage, this);
    Channel* channel = new Channel(m_socketPair[1], FDEvent::ReadEvent,
        obj, nullptr, nullptr, this);
#endif
    // channel 添加到任务队列
    addTask(channel, ElemType::ADD);
}

EventLoop::~EventLoop()
{
}

int EventLoop::run()
{
    this->m_isQuit = false;
    // 比较线程ID是否正常
    if (this->m_threadID != std::this_thread::get_id())
    {
        return -1;
    }
    // 循环进行事件处理
    while (!m_isQuit)
    {
        this->m_dispatcher->dispatch();    // 超时时长 2s
        processTaskQ();
    }
    return 0;
}

int EventLoop::eventActive(int fd, int event)
{
    if (fd < 0)
    {
        return -1;
    }
    // 取出channel
    Channel* channel = this->m_channelMap[fd];
    assert(channel->getSocket() == fd);
    if (event & (int)FDEvent::ReadEvent && channel->readCallback)
    {
        channel->readCallback(const_cast<void*>(channel->getArg()));
    }
    if (event & (int)FDEvent::WriteEvent && channel->writeCallback)
    {
        channel->writeCallback(const_cast<void*>(channel->getArg()));
    }
    return 0;
}

int EventLoop::addTask(Channel* channel, ElemType type)
{
    // 加锁, 保护共享资源
    this->m_mutex.lock();
    // 创建新节点
    ChannelElement* node = new ChannelElement;
    node->channel = channel;
    node->type = type;
    this->m_taskQ.push(node);
    this->m_mutex.unlock();
    // 处理节点
    /*
    * 细节:
    *   1. 对于链表节点的添加: 可能是当前线程也可能是其他线程(主线程)
    *       1). 修改fd的事件, 当前子线程发起, 当前子线程处理
    *       2). 添加新的fd, 添加任务节点的操作是由主线程发起的
    *   2. 不能让主线程处理任务队列, 需要由当前的子线程取处理
    */
    if (this->m_threadID == std::this_thread::get_id())
    {
        // 当前子线程(基于子线程的角度分析)
        processTaskQ();
    }
    else
    {
        // 主线程 -- 告诉子线程处理任务队列中的任务
        // 1. 子线程在工作 2. 子线程被阻塞了:select, poll, epoll
        taskWakeup();
    }
    return 0;
}

int EventLoop::processTaskQ()
{
    // 取出头结点
    while (!this->m_taskQ.empty())
    {
        this->m_mutex.lock();
        ChannelElement* node = this->m_taskQ.front();
        this->m_taskQ.pop();  // 删除节点
        this->m_mutex.unlock();
        Channel* channel = node->channel;
        if (node->type == ElemType::ADD)
        {
            // 添加
            add(channel);
        }
        else if (node->type == ElemType::DELETE)
        {
            // 删除
            remove(channel);
        }
        else if (node->type == ElemType::MODIFY)
        {
            // 修改
            modify(channel);
        }
        delete node;
    }
    return 0;
}

int EventLoop::add(Channel* channel)
{
    int fd = channel->getSocket();
    // 找到fd对应的数组元素位置, 并存储
    if (this->m_channelMap.find(fd) == this->m_channelMap.end())
    {
        this->m_channelMap.insert(std::make_pair(fd, channel));
        this->m_dispatcher->setChannel(channel);
        int ret = this->m_dispatcher->add();
        return ret;
    }
    return -1;
}

int EventLoop::remove(Channel* channel)
{
    int fd = channel->getSocket();
    if (this->m_channelMap.find(fd) == this->m_channelMap.end())
    {
        return -1;
    }
    this->m_dispatcher->setChannel(channel);
    int ret = this->m_dispatcher->remove();
    return ret;
}

int EventLoop::modify(Channel* channel)
{
    int fd = channel->getSocket();
    if (this->m_channelMap.find(fd) == this->m_channelMap.end())
    {
        return -1;
    }
    this->m_dispatcher->setChannel(channel);
    int ret = this->m_dispatcher->modify();
    return ret;
}

int EventLoop::readLocalMessage(void* arg)
{
    EventLoop* evLoop = static_cast<EventLoop*>(arg);
    char buf[256];
    read(evLoop->m_socketPair[1], buf, sizeof(buf));
    return 0;
}

void EventLoop::taskWakeup()
{
    const char* msg = "我是要成为海贼王的男人!!!";
    write(this->m_socketPair[0], msg, strlen(msg));
}

int EventLoop::freeChannel(Channel* channel)
{
    // 删除 channel 和 fd 的对应关系
    auto it = this->m_channelMap.find(channel->getSocket());
    if (it != this->m_channelMap.end())
    {
        this->m_channelMap.erase(it);
        close(channel->getSocket());
        delete channel;
    }
    return 0;
}

int EventLoop::readMessage()
{
    char buf[256];
    read(this->m_socketPair[1], buf, sizeof(buf));
    return 0;
}
