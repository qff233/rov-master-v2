#include "event.h"
#include "ms5837.h"

class EventMS5837 : public EventBase {
public:
    EventMS5837();
    void process() noexcept override;
};
