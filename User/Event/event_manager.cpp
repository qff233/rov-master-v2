#include "event_manager.h"

#define LOG_TAG "EventManager"
#define LOG_LVL ELOG_LVL_DEBUG

#include <elog.h>
#include <unistd.h>

EventManager::EventManager(const std::string& name, int time_out) noexcept
    :m_name(name),
     m_timeOut(time_out)
{
    m_epfd = epoll_create(25);
}

EventManager::~EventManager() noexcept {
    close(m_epfd);
    for(const auto& [key, value] : m_cbs) {
        close(key);
    }
}

void EventManager::run() noexcept
{
    static epoll_event events[30];
    while(true) {
        int event_count = epoll_wait(m_epfd, events, 30, m_timeOut);
	//if(event_count == 0) log_d("has idea time");
        for(int i = 0; i < event_count; ++i) {
            int fd = events[i].data.fd;
            m_cbs[fd]->process();
        }
    }
}

void EventManager::addEvent(EventBase::ptr event, EventType type) {
    int fd = event->getFd();
    epoll_event ep_event;
    ep_event.data.fd = fd;
    ep_event.events = (int)type | EPOLLET;

    epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ep_event);

    m_cbs[fd] = std::move(event);
}

void EventManager::addEvent(int fd, std::function<void()> cb, EventType type)
{
    EventBase::ptr event = std::make_unique<EventFunction>(fd, cb);
    this->addEvent(std::move(event), type);
}

void EventManager::delEvent(int fd) noexcept
{
    auto result = m_cbs.find(fd);
    if(result == m_cbs.end()) return;

    m_cbs.erase(fd);
    epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr);
}
