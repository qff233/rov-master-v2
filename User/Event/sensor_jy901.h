#include "event.h"
#include "jy901.h"

class EventJY901 : public EventBase {
public:
    EventJY901();
    void process() noexcept override;
};
