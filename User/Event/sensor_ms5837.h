#include "event.h"
#include "ms5837.h"

class EventMS5837 : public Event {
public:
    EventMS5837();
    void process() noexcept override;
private:
    MS5837 m_ms5837;
};
