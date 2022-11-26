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
    int jy_fd = m_jy901.getFd();
    while(serialDataAvail(jy_fd)) {
        uint8_t data = serialGetchar(jy_fd);
        if(!m_jy901.inputData(data)) break;
    }
    std::cout << m_jy901.getYaw() << std::endl;
}
