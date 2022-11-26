#ifndef __USER_EVENT_EVENT_MANAGER_H__
#define __USER_EVENT_EVENT_MANAGER_H__

#include <sys/epoll.h>
#include <map>
#include <functional>
#include <string>
#include <memory>

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
    
    EventManager(const std::string& name, int time_out = 1000) noexcept;  //默认超时�1 秒
    ~EventManager() noexcept;

    void run() noexcept;  // 这个函数要新开辟个线程跑 专门处理事件

    void addEvent(Event::ptr event, EventType type = EventType::READ);
    void addEvent(int fd, std::function<void()> event, EventType type = EventType::READ);
    void delEvent(int fd) noexcept;
private:
    std::string m_name;
    int m_epfd;
    int m_timeOut;
    std::map<int, Event::ptr> m_cbs;
};

#endif
