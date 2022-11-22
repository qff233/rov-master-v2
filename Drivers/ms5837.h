#ifndef _MS5837_H
#define _MS5837_H

#include "../User/config.h"

#define MS5837_UART_DEV "/dev/ttyS1"
#define MS5837_UART_BAUD 115200

#define MS5837_PACKET_LENGTH 11
#define MS5837_CMD_LENGTH 5

struct ms5837_data
{
    float temperature;
    float depth;
};

class MS5837
{
public:
    MS5837();

    void inputData(std::uint8_t data) noexcept;
    const ms5837_data& getData() const noexcept;
    int getFd() const noexcept;  //获取fd

private:
    ms5837_data m_sensorData;
    std::uint8_t m_rxBuffer[20] = {0};
    std::uint8_t m_rxData[20] = {0};
    int m_serialFd;

    void rawToData(std::uint8_t packet_length) noexcept;
};

#endif //_MS5837_H
