#include "SelectDispatcher.h"

SelectDispatcher::SelectDispatcher(EventLoop* evloop) : Dispatcher(evloop)
{
    FD_ZERO(&this->m_readSet);
    FD_ZERO(&this->m_writeSet);
    this->m_name = "Select";
}

SelectDispatcher::~SelectDispatcher()
{

}

int SelectDispatcher::add()
{
    if (this->m_channel->getSocket() >= this->m_maxSize)
    {
        return -1;
    }
    setFdSet();
    return 0;
}

int SelectDispatcher::remove()
{
    clearFdSet();
    // 通过 channel 释放对应的 TcpConnection 资源
    this->m_channel->destroyCallback(const_cast<void*>(this->m_channel->getArg()));

    return 0;
}

int SelectDispatcher::modify()
{
    setFdSet();
    clearFdSet();
    return 0;
}

int SelectDispatcher::dispatch(int timeout)
{
    timeval val;
    val.tv_sec = timeout;
    val.tv_usec = 0;
    fd_set rdtmp = this->m_readSet;
    fd_set wrtmp = this->m_writeSet;
    int count = select(this->m_maxSize, &rdtmp, &wrtmp, nullptr, &val);
    if (count == -1)
    {
        perror("select");
        exit(0);
    }
    for (int i = 0; i < this->m_maxSize; i ++ )
    {
        if (FD_ISSET(i, &rdtmp))
        {
            // 处理读事件
            this->m_evLoop->eventActive(i, static_cast<int>(FDEvent::ReadEvent));
        }

        if (FD_ISSET(i, &wrtmp))
        {
            // 处理写事件
            this->m_evLoop->eventActive(i, static_cast<int>(FDEvent::WriteEvent));
        }
    }
    return 0;
}

void SelectDispatcher::setFdSet()
{
    if (this->m_channel->getEvent() & static_cast<int>(FDEvent::ReadEvent))
    {
        FD_SET(this->m_channel->getSocket(), &this->m_readSet);
    }
    if (this->m_channel->getEvent() & static_cast<int>(FDEvent::WriteEvent))
    {
        FD_SET(this->m_channel->getSocket(), &this->m_writeSet);
    }
}

void SelectDispatcher::clearFdSet()
{
    if (this->m_channel->getEvent() & static_cast<int>(FDEvent::ReadEvent))
    {
        FD_CLR(this->m_channel->getSocket(), &this->m_readSet);
    }
    if (m_channel->getEvent() & static_cast<int>(FDEvent::WriteEvent))
    {
        FD_CLR(this->m_channel->getSocket(), &this->m_writeSet);
    }
}