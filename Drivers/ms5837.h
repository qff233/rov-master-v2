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

    int inputData(uint8_t data) noexcept; 
    const Ms5837Data& getData() const noexcept;
    int getFd() const noexcept;  //获取fd

    bool isValid() const noexcept;
    
    float getTemperature() const noexcept;
    float getDepth() const noexcept;
private:
    Ms5837Data m_sensorData;
    std::string m_rxBuffer;
    int m_serialFd;

    void rawToData() noexcept;
};

#endif //__DRIVERS_MS5837_H__
