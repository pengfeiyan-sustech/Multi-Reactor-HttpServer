#include "PollDispatcher.h"


PollDispatcher::PollDispatcher(EventLoop* evLoop) : Dispatcher(evLoop)
{
    this->m_maxfd = 0;
    this->m_fds = new pollfd[this->m_maxNode];
    for (int i = 0; i < m_maxNode; i ++ )
    {
        this->m_fds[i].fd = -1;
        this->m_fds[i].events = 0;
        this->m_fds[i].revents = 0;
    }
    this->m_name = "Poll";
}

PollDispatcher::~PollDispatcher()
{
    delete[] this->m_fds;
}

int PollDispatcher::add()
{
    int events = 0;
    if (this->m_channel->getEvent() & static_cast<int>(FDEvent::ReadEvent))
    {
        events |= POLLIN;
    }
    if (this->m_channel->getEvent() & static_cast<int>(FDEvent::WriteEvent))
    {
        events |= POLLOUT;
    }
    int i = 0;
    for (; i < this->m_maxNode; i ++ )
    {
        if (this->m_fds[i].fd == -1)
        {
            this->m_fds[i].events = events;
            this->m_fds[i].fd = this->m_channel->getSocket();
            this->m_maxfd = (i > this->m_maxfd) ? i : this->m_maxfd;
            break;
        }
    }
    if (i >= this->m_maxNode)
    {
        return -1;
    }
    return 0;
}

int PollDispatcher::remove()
{
    int i = 0;
    for (; i < this->m_maxNode; i ++ )
    {
        if (this->m_fds[i].fd == this->m_channel->getSocket())
        {
            this->m_fds[i].events = 0;
            this->m_fds[i].revents = 0;
            this->m_fds[i].fd = -1;
            break;
        }
    }
    this->m_channel->destroyCallback(const_cast<void*>(this->m_channel->getArg()));
    if (i >= this->m_maxNode)
    {
        return -1;
    }
    return 0;
}

int PollDispatcher::modify()
{
    int events = 0;
    if (this->m_channel->getEvent() & static_cast<int>(FDEvent::ReadEvent))
    {
        events |= POLLIN;
    }
    if (this->m_channel->getEvent() & static_cast<int>(FDEvent::WriteEvent))
    {
        events |= POLLOUT;
    }
    int i = 0;
    for (; i < this->m_maxNode; i ++ )
    {
        if (this->m_fds[i].fd == this->m_channel->getSocket())
        {
            this->m_fds[i].events = events;
            break;
        }
    }
    if (i >= this->m_maxNode)
    {
        return -1;
    }
    return 0;
}

int PollDispatcher::dispatch(int timeout)
{
    int count = poll(this->m_fds, this->m_maxfd + 1, timeout * 1000);
    if (count == -1)
    {
        perror("poll");
        exit(0);
    }
    for (int i = 0; i < count; i ++ )
    {   
        if (m_fds[i].fd == -1)
        {
            continue;
        }
        if (this->m_fds[i].revents & POLLIN)
        {
            // 处理读事件
            this->m_evLoop->eventActive(this->m_fds[i].fd, static_cast<int>(FDEvent::ReadEvent));
        }
        if (this->m_fds[i].revents & POLLOUT)
        {
            // 处理写事件
            this->m_evLoop->eventActive(this->m_fds[i].fd, static_cast<int>(FDEvent::WriteEvent));
        }
    }
    return 0;
}