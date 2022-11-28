#ifndef __USER_EVENT_EVENT_H__
#define __USER_EVENT_EVENT_H__

#include <memory>
#include <functional>

class EventBase 
{
public:
    using ptr = std::unique_ptr<EventBase>;

    EventBase(int fd = 0) : m_fd(fd) { }
    virtual ~EventBase() = default;

    virtual void process() noexcept = 0;

    int getFd() const noexcept { return m_fd; }
protected:
    int m_fd;
};

class EventFunction : public EventBase
{
public:
    EventFunction(int fd, std::function<void()> cb)
        : EventBase(fd), m_cb(cb) { }

    void process() noexcept override { m_cb(); }
private:
    std::function<void()> m_cb;
};

#endif
