#include "event.h"

Event::Event(int fd)
    : m_fd(fd)
{
}

int Event::getFd() const 
{
    return m_fd;
}

EventFunction::EventFunction(int fd, std::function<void()> cb) 
    : Event(fd),
      m_cb(cb)
{
}

void EventFunction::process() noexcept
{
    m_cb();
}
