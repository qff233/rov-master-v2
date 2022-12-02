#include <cstring>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <easylogging++.h>

#include "sf0050.h"

#define SF0050_UART_DEV "/dev/ttyS1"
#define SF0050_UART_BAUD 9600

#define SF0050_HEAD_BYTE 0xAB

#define SF0050_FRI_DISTANCE_HB_NUM 4
#define SF0050_FRI_DISTANCE_LB_NUM 5
#define SF0050_PEAK_DISTANCE_HB_NUM 8
#define SF0050_PEAK_DISTANCE_LB_NUM 9
#define SF0050_LAST_DISTANCE_HB_NUM 12
#define SF0050_LAST_DISTANCE_LB_NUM 13

SF0050::SF0050()
{
    m_serialFd = serialOpen(SF0050_UART_DEV, SF0050_UART_BAUD);
    if (m_serialFd < 0)
    {
        LOG(ERROR) << "Unable to get the fd";
        return;
    }
    // 后面要加初始化标志位

    m_sensorData.peak_target_distance = NAN;

    for (i = 0; i < SF0050_CMD_LENGTH; i++)
        serialPutchar(m_serialFd, SET_TIMING_MEASURE[i]);
}

bool SF0050::isValid() const noexcept
{
    return !std::isnan(m_sensorData.peak_target_distance);
}

int SF0050::inputData(uint8_t data) noexcept
{
    static uint8_t rxCount = 0;
    m_rxBuffer[rxCount++] = data;
    if (m_rxBuffer[0] != SF0050_HEAD_BYTE)
    {
        rxCount = 0;
        return -1;
    }
    if (SF0050_PACKET_LENGTH == rxCount)
    {
        if (!verifyRawData())
        {
            rxCount = 0;
            return;
        }
        rawToData();
        rxCount = 0;
        return 0;
    }
    return 1;
}

void SF0050::rawToData() noexcept
{
    m_sensorData.first_target_distance = (uint32_t)(m_rxBuffer[SF0050_FRI_DISTANCE_HB_NUM] * 256 + m_rxBuffer[SF0050_FRI_DISTANCE_LB_NUM]);
    m_sensorData.peak_target_distance = (uint32_t)(m_rxBuffer[SF0050_PEAK_DISTANCE_HB_NUM] * 256 + m_rxBuffer[SF0050_PEAK_DISTANCE_LB_NUM]);
    m_sensorData.last_target_distance = (uint32_t)(m_rxBuffer[SF0050_LAST_DISTANCE_HB_NUM] * 256 + m_rxBuffer[SF0050_LAST_DISTANCE_LB_NUM]);
}

bool SF0050::verifyRawData() noexcept
{
    uint8_t real_code = m_rxBuffer[0];
    for (uint8_t i = 1; i < SF0050_PACKET_LENGTH - 1; i++)
    {
        real_code ^= m_rxBuffer[i];
    }
    if (m_rxBuffer[SF0050_PACKET_LENGTH - 1] == real_code)
        return true;
    return false;
}

const SF0050Data &SF0050::getData() const noexcept
{
    return m_sensorData;
}

int SF0050::getFd() const noexcept
{
    return m_serialFd;
}

uint32_t SF0050::getDistance() const noexcept
{
    return m_sensorData.peak_target_distance;
}
