#ifndef EPOLLDISPATCHER_H
#define EPOLLDISPATCHER_H

#include "Dispatcher.h"
#include <sys/epoll.h>
#include "EventLoop.h"
#include <unistd.h>


/**
 * @brief 基于epoll模型
 */
class EpollDispatcher : public Dispatcher
{
public:
    /**
     * @brief 子类构造函数
     */
    EpollDispatcher(EventLoop* evLoop);
    /**
     * @brief 析构函数
     */
    ~EpollDispatcher();
    /**
     * @brief 添加Socket（被封装为channel类型），添加时需要知道epoll节点，该节点保存于EventLoop类
     */
    int add() override;
    /**
     * @brief 移除Socket（被封装为channel类型），移除时需要知道epoll节点，该节点保存于EventLoop类
     */
    int remove() override;
    /**
     * @brief 修改Socket的监听事件（被封装为channel类型），修改时需要知道epoll节点，该节点保存于EventLoop类
     */
    int modify() override;
    /**
     * @brief 运行事件监测，执行回调函数
     * @param[in] timeout 超时时间，单位：秒
     */
    int dispatch(int timeout = 2) override;
    /**
     * @brief 构造一个epoll_event，根据op对Socket进行操作
     * @param[in] op 宏定义，对fd进行的操作
     */
    int epollCtl(int op);
private:
    // epoll红黑树节点
    int m_epfd;
    // 存放就绪事件的数组
    epoll_event* m_events;
    // 最大就绪事件数
    const int m_maxNode = 520;
};

#endif