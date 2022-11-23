#ifndef __DRIVERS_MS5837_H__
#define __DRIVERS_MS5837_H__

#include "User/config.h"

struct Ms5837Data
{
    float temperature;
    float depth;
};

class MS5837
{
public:
    MS5837();

    void inputData(uint8_t data) noexcept;
    const Ms5837Data& getData() const noexcept;
    int getFd() const noexcept;  //获取fd

private:
    Ms5837Data m_sensorData;
    uint8_t m_rxBuffer[20] = {0};
    uint8_t m_rxData[20] = {0};
    int m_serialFd;

    void rawToData(uint8_t packet_length) noexcept;
};

#endif //__DRIVERS_MS5837_H__
