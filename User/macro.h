#ifndef __USER_MACRO_H__
#define __USER_MACRO_H__

#include "utils.h"
#include "Event/event.h"
#include "Event/event_manager.h"

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define rad2Deg(rad) (rad * 180.0f / M_PI)
#define deg2Rad(deg) (deg * M_PI / 180.0f)

template<class T>
void add_event(EventManager::EventType type = EventManager::EventType::READ)
{
    EventBase::ptr event = std::make_unique<T>();
    Global<EventManager>::Get()->addEvent(std::move(event), type);
}

#define ADD_METHOD(name, ...) \
    Global<RPCServer>::Get()->addMethod(#name, jsonrpccxx::GetHandle(Method::name), {__VA_ARGS__})

#define BITS(x) (1 << x)

#endif