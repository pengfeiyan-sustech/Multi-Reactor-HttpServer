#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include "Channel.h"
#include "EventLoop.h"

class EventLoop;

/**
 * @brief IO多路复用的基类，父类指针或引用指向子类对象
 */
class Dispatcher
{
public:
    /**
     * @brief 构造函数，初始化epoll，poll，select 需要的数据块
     * @param[in] evLoop 指定该Dispatcher属于哪个反应堆，任务要存储到反应堆的任务队列里面
     */
    Dispatcher(EventLoop* evLoop);
    /**
     * @brief 虚析构
     */
    virtual ~Dispatcher();
    /**
     * @brief 添加Socket（被封装为channel类型）
     */
    virtual int add();
    /**
     * @brief 移除Socket（被封装为channel类型）
     */
    virtual int remove();
    /**
     * @brief 修改Socket的监听事件（被封装为channel类型）
     */
    virtual int modify();
    /**
     * @brief 运行事件监测，需要访问数据块
     * @param[in] timeout 超时时间，单位：秒
     */
    virtual int dispatch(int timeout = 2);
    /**
     * @brief 更新要进行操作的channel
     * @param[in] channel 要更新的Channel类指针
     */
    inline void setChannel(Channel* channel)
    {
        this->m_channel = channel;
    }

protected:      // 外部无法访问，子类可以继承
    // 保存父类指向的具体IO多路复用模型的名字
    std::string m_name = std::string();
    // channel是动态的，每次管理的channel都不一样
    Channel* m_channel;
    // 反应堆
    EventLoop* m_evLoop;
};






#endif