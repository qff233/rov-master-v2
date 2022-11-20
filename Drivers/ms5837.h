//
// Created by fxf on 22-11-20.
//

#ifndef _MS5837_H
#define _MS5837_H

#include "../User/config.h"

#define MS5837_UART_DEV "/dev/ttyS1" // JY901 UART 号
#define MS5837_UART_BAUD 115200         // JY901 UART 波特率

#define MS5837_PACKET_LENGTH 11   // JY901 数据包长度
#define MS5837_CMD_LENGTH 5       // JY901 命令长度

struct ms5837_t
{
    float temperature;
    float depth;
};

class MS5837
{
public:
    MS5837();

    void inputData(uint8_t data) noexcept;
    const ms5837_t& getData() const noexcept;
    int getFd() const noexcept;  //获取fd

private:
    ms5837_t m_sensorData;
    uint8_t m_rxBuffer[20] = {0};
    uint8_t m_rxData[20] = {0};
    int m_serialFd;

    void rawToData(uint8_t packet_length) noexcept;
};

#endif //_MS5837_H
