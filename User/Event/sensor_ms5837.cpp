#include "sensor_ms5837.h"
#include <wiringPi.h>
#include <wiringSerial.h>

#include <iostream>

EventMS5837::EventMS5837()
{
    m_fd = m_ms5837.getFd();
}

void EventMS5837::process() noexcept
{
    while(serialDataAvail(m_fd)) {
        uint8_t data = serialGetchar(m_fd);
        if(!m_ms5837.inputData(data)) break;
    }
    std::cout << m_ms5837.getDepth() << std::endl;
}
