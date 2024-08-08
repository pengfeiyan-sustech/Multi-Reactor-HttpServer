#ifndef POLLDISPATCHER_H
#define POLLDISPATCHER_H

#include "Dispatcher.h"
#include <sys/poll.h>
#include "EventLoop.h"
#include <unistd.h>


/**
 * @brief 基于poll模型
 */
class PollDispatcher : public Dispatcher
{
public:
    /**
     * @brief 子类构造函数
     */
    PollDispatcher(EventLoop* evLoop);
    /**
     * @brief 析构函数
     */
    ~PollDispatcher();
    /**
     * @brief 添加Socket（被封装为channel类型）
     */
    int add() override;
    /**
     * @brief 移除Socket（被封装为channel类型）
     */
    int remove() override;
    /**
     * @brief 修改Socket的监听事件（被封装为channel类型）
     */
    int modify() override;
    /**
     * @brief 运行事件监测，执行回调函数
     * @param[in] timeout 超时时间，单位：秒
     */
    int dispatch(int timeout = 2) override;

private:
    // 遍历数组时指定的范围
    int m_maxfd;
    // 存放事件的数组
    pollfd* m_fds;
    // 最大就绪事件数
    const int m_maxNode = 1024;
};

#endif