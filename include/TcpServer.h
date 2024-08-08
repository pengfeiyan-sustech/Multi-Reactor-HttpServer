#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "EventLoop.h"
#include "ThreadPool.h"

class TcpServer
{
public:
    /**
     * @brief 构造函数
     */
    TcpServer(unsigned short port, int threadNum);
    /**
     * @brief 初始化监听
     */
    void setListen();
    /**
     * @brief 启动服务器
     */
    void run();
    /**
     * @brief 监听套接字的读回调函数
     */
    static int acceptConnection(void* arg);

private:
    int m_threadNum;
    EventLoop* m_mainLoop;
    ThreadPool* m_threadPool;
    int m_lfd;
    unsigned short m_port;
};

#endif