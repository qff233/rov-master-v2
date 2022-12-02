#ifndef __USER_EVENT_SNSOR_JY901_H__
#define __USER_EVENT_SNSOR_JY901_H__

#include "event.h"
#include "jy901.h"

class EventJY901 : public EventBase {
public:
    EventJY901();
    void process() noexcept override;
};

#endif