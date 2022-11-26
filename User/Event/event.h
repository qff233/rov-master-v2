#ifndef __USER_EVENT_EVENT_H__
#define __USER_EVENT_EVENT_H__

#include <memory>
#include <functional>

class Event 
{
public:
    using ptr = std::unique_ptr<Event>;

    Event(int fd = 0);
    virtual ~Event() = default;

    virtual void process() noexcept = 0;

    int getFd() const;
protected:
    int m_fd;
};

class EventFunction : public Event
{
public:
    EventFunction(int fd, std::function<void()> cb);

    void process() noexcept override;
private:
    std::function<void()> m_cb;
};

#endif
