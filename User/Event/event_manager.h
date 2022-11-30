#ifndef __USER_EVENT_EVENT_MANAGER_H__
#define __USER_EVENT_EVENT_MANAGER_H__

#include <sys/epoll.h>
#include <map>
#include <functional>
#include <string>
#include <memory>
#include <thread>

#include "event.h"

class EventManager 
{
public:
    enum class EventType 
    {
        READ         = EPOLLIN,
        WRITE        = EPOLLOUT,
        READANDWRITE = EPOLLIN | EPOLLOUT
    };
    
    EventManager(const std::string& name, int time_out = 1000) noexcept;  //默认超时1 秒
    ~EventManager() noexcept;

    void start() noexcept;
    void stop() noexcept;

    void addEvent(EventBase::ptr event, EventType type = EventType::READ);
    void addEvent(int fd, std::function<void()> event, EventType type = EventType::READ);
    void delEvent(int fd) noexcept;
private:
    void run() noexcept;
private:
    std::string m_name;
    int m_epfd;
    int m_timeOut;
    std::map<int, EventBase::ptr> m_cbs;
    std::thread m_thread;
    bool m_isRunning;
};

#endif
