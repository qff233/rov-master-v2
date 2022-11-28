#include "event.h"
#include "pca9685.h"

class EventPCA9685 : public EventBase {
public:
    EventPCA9685();
    void process() noexcept override;
};
