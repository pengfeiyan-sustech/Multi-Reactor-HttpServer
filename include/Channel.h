#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>

// 定义函数指针
// typedef int(*handleFunc)(void* arg);
// using handleFunc = int(*)(void*);

// 事件强类型枚举
enum class FDEvent
{
    TimeOut = 0x01,
    ReadEvent = 0x02,
    WriteEvent = 0x04
};

/**
 * @brief Socket相关内容的管理
 */
class Channel
{
public:
    using handleFunc = std::function<int(void*)>;
    /**
     * @brief 构造函数
     * @param[in] fd socket套接字
     * @param[in] events 要监听的事件
     * @param[in] readFunc 读事件回调
     * @param[in] writeCallback 写事件回调
     * @param[in] arg 回调函数的参数
     */
    Channel(int fd, FDEvent events, 
            handleFunc readFunc, handleFunc writeFunc, 
            handleFunc destroyFunc, void* arg);
    /**
     * @brief 析构函数
     */
    ~Channel();
    /**
     * @brief 根据flag决定是否监听写事件
     * @param[in] flag 为true，则监听写事件
     */
    void writeEventEnable(bool flag);
    /**
     * @brief 判断是否需要检测写事件
     */
    bool isWriteEventEnable();
    // 回调函数
    handleFunc readCallback;
    handleFunc writeCallback;
    handleFunc destroyCallback;
    /**
     * @brief 取出监听的事件
     */
    inline int getEvent() { return this->m_events; }
    /**
     * @brief 取出Socket套接字
     */
    inline int getSocket() { return this->m_fd; }
    /**
     * @brief 取出私有成员的值
     */
    inline const void* getArg() { return this->m_arg; }
private:
    // 文件描述符
    int m_fd;
    // 事件
    int m_events;
    // 回调函数的参数
    void* m_arg;
};

#endif