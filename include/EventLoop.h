#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Dispatcher.h"
#include "Channel.h"
#include <thread>
#include <queue>
#include <map>
#include <mutex>


// 处理该节点中的channel的方式
enum class ElemType:char{ADD, DELETE, MODIFY};
// 定义任务队列的节点
struct ChannelElement
{
    ElemType type;   // 如何处理该节点中的channel
    Channel* channel;
};
class Dispatcher;

class EventLoop
{
public:
    /**
     * @brief 委托构造函数，用于主线程
     */
    EventLoop();
    /**
     * @brief 构造函数，用于子线程
     */
    EventLoop(const std::string threadName);
    /**
     * @brief 析构函数
     */
    ~EventLoop();
    /**
     * @brief 启动反应堆模型
     */
    int run();
    /**
     * @brief 处理别激活的文件fd
     */
    int eventActive(int fd, int event);
    /**
     * @brief 添加任务到任务队列
     */
    int addTask(struct Channel* channel, ElemType type);
    /**
     * @brief 处理任务队列中的任务
     */
    int processTaskQ();
    /**
     * @brief 处理dispatcher中的节点
     */
    int add(Channel* channel);
    /**
     * @brief 处理dispatcher中的节点
     */
    int remove(Channel* channel);
    /**
     * @brief 处理dispatcher中的节点
     */
    int modify(Channel* channel);
    /**
     * 释放channel
     */
    int freeChannel(Channel* channel);
    /**
     * @brief 使用可调用对象包装器进行绑定
     */
    int readMessage();
    /**
     * @brief 返回线程ID
     */
    inline std::thread::id getThreadID()
    {
        return m_threadID;
    }
    /**
     * @brief 返回线程名字
     */
    inline std::string getThreadName()
    {
        return m_threadName;
    }
    /**
     * @brief 本地套接字通信的读回调，函数指针需要指向类内静态成员函数
     */
    static int readLocalMessage(void* arg);

private:
    /**
     * @brief 唤醒epoll阻塞
     */
    void taskWakeup();

private:
    // 标记反应堆是否退出
    bool m_isQuit;
    // 该指针指向子类的实例 epoll, poll, select
    Dispatcher* m_dispatcher;
    // 任务队列
    std::queue<ChannelElement*> m_taskQ;
    // map
    std::map<int, Channel*> m_channelMap;
    // 线程id, name, mutex
    std::thread::id m_threadID;
    std::string m_threadName;
    std::mutex m_mutex;
    int m_socketPair[2];  // 存储本地通信的fd 通过socketpair 初始化
};



#endif