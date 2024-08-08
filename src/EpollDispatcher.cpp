#include "../include/EpollDispatcher.h"


EpollDispatcher::EpollDispatcher(EventLoop* evLoop) : Dispatcher(evLoop)
{
    this->m_epfd = epoll_create1(10);
    if (this->m_epfd == -1)
    {
        perror("epoll_create");
        exit(0);
    }
    this->m_events =new epoll_event[this->m_maxNode];
    this->m_name = "Epoll";
}

EpollDispatcher::~EpollDispatcher()
{
    close(this->m_epfd);
    delete[] this->m_events;
}

int EpollDispatcher::epollCtl(int op)
{
    epoll_event ev;
    ev.data.fd = this->m_channel->getSocket();
    int events = 0;
    if (this->m_channel->getEvent() & static_cast<int>(FDEvent::ReadEvent))
    {
        events |= EPOLLIN;
    }
    if (this->m_channel->getEvent() & static_cast<int>(FDEvent::WriteEvent))
    {
        events |= EPOLLOUT;
    }
    ev.events = events;
    int ret = epoll_ctl(this->m_epfd, op, this->m_channel->getSocket(), &ev);
    return ret;
}

int EpollDispatcher::add()
{
    int ret = this->epollCtl(EPOLL_CTL_ADD);
    if (ret == -1)
    {
        perror("epoll_crl add");
        exit(0);
    }
    return ret;
}

int EpollDispatcher::remove()
{
    int ret = this->epollCtl(EPOLL_CTL_DEL);
    if (ret == -1)
    {
        perror("epoll_crl delete");
        exit(0);
    }
    this->m_channel->destroyCallback(const_cast<void*>(this->m_channel->getArg()));
    return ret;
}

int EpollDispatcher::modify()
{
    int ret = this->epollCtl(EPOLL_CTL_MOD);
    if (ret == -1)
    {
        perror("epoll_crl modify");
        exit(0);
    }
    return ret;
}

int EpollDispatcher::dispatch(int timeout)
{
    int count = epoll_wait(this->m_epfd, this->m_events, this->m_maxNode, timeout * 1000);
    for (int i = 0; i < count; i ++ )
    {
        int events = this->m_events[i].events;
        int fd = this->m_events[i].data.fd;
        if (events & EPOLLERR || events & EPOLLHUP)
        {
            // 客户端断开了连接，删除fd
        }
        if (events & EPOLLIN)
        {
            // 处理读事件
            this->m_evLoop->eventActive(fd, static_cast<int>(FDEvent::ReadEvent));
        }
        if (events & EPOLLOUT)
        {
            // 处理写事件
            this->m_evLoop->eventActive(fd, static_cast<int>(FDEvent::WriteEvent));
        }
    }
    return 0;
}