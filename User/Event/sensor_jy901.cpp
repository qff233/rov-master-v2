#include "sensor_jy901.h"
#include <wiringPi.h>
#include <wiringSerial.h>

#include "User/utils.h"

EventJY901::EventJY901()
{
    m_fd = Global<JY901>::Get()->getFd();
}

void EventJY901::process() noexcept
{
    while(serialDataAvail(m_fd)) {
        uint8_t data = serialGetchar(m_fd);
        if(!Global<JY901>::Get()->inputData(data)) break;
    }
}
