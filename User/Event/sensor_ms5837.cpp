#include "sensor_ms5837.h"
#include <wiringPi.h>
#include <wiringSerial.h>

#include "User/utils.h"

EventMS5837::EventMS5837()
{
    m_fd = Global<MS5837>::Get()->getFd();
}

void EventMS5837::process() noexcept
{
    while(serialDataAvail(m_fd)) {
        uint8_t data = serialGetchar(m_fd);
        if(!Global<MS5837>::Get()->inputData(data)) break;
    }
}
