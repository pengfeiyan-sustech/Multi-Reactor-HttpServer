#include "../include/Channel.h"

Channel::Channel(int fd, FDEvent events, 
                handleFunc readFunc, handleFunc writeFunc, 
                handleFunc destroyFunc, void* arg) : 
                m_fd(fd), m_events(static_cast<int>(events)),
                readCallback(readFunc),
                writeCallback(writeFunc),
                destroyCallback(destroyFunc),
                m_arg(arg) {}

Channel::~Channel() {}

void Channel::writeEventEnable(bool flag)
{
    if (flag)
    {
        this->m_events |= static_cast<int>(FDEvent::WriteEvent);
    }
    else
    {
        this->m_events &= ~static_cast<int>(FDEvent::WriteEvent);
    }
}

bool Channel::isWriteEventEnable()
{
    return this->m_events & static_cast<int>(FDEvent::WriteEvent);
}