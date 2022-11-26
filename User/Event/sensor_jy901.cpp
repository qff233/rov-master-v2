#include "sensor_jy901.h"
#include <wiringPi.h>
#include <wiringSerial.h>

#include <iostream>

EventJY901::EventJY901()
{
    m_fd = m_jy901.getFd();
}

void EventJY901::process() noexcept
{
    while(serialDataAvail(m_fd)) {
        uint8_t data = serialGetchar(m_fd);
        if(!m_jy901.inputData(data)) break;
    }
    std::cout << "jy901:" << m_jy901.getYaw() << std::endl;
}
