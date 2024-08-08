#ifndef SELECTDISPATCHER_H
#define SELECTDISPATCHER_H


#include "Channel.h"
#include "EventLoop.h"
#include "Dispatcher.h"
#include <string>
#include <sys/select.h>

/**
 * @brief 基于select模型
 */
class SelectDispatcher : public Dispatcher
{
public:
    SelectDispatcher(EventLoop* evloop);
    ~SelectDispatcher();
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
    void setFdSet();
    void clearFdSet();

private:
    fd_set m_readSet;
    fd_set m_writeSet;
    const int m_maxSize = 1024;
};



#endif