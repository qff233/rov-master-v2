//
// Created by fxf on 22-11-19.
//

#include <string>

#include "jy901.h"

JY901::JY901(const std::string &dev, const unsigned int baud)
: m_uartBaud(baud),
  m_uartDev(dev)
  {
    m_serialFd = serialOpen(JY901_UART_DEV, JY901_UART_BAUD);
    if (m_serialFd < 0)
        log_e("JY901 uart init failed");
}

void J901::rawToData(void) noexcept
{
    switch (m_rxBuffer[1])
    {
        case 0x51:
        {
            m_sensorData->acc.x = (m_sensorRaw.stcAcc.a[0] / 32768.0f*16); // 32768*16
            m_sensorData->acc.y = (m_sensorRaw.stcAcc.a[1] / 32768.0f*16);
            m_sensorData->acc.z = (m_sensorRaw.stcAcc.a[2] / 32768.0f*16);
            m_sensorData->temperature = (m_sensorRaw.stcAcc.T / 100.0f);
            //printf("acc %0.2f %0.2f %0.2f  %0.2fC\n", m_sensorData->acc.x, m_sensorData->acc.y, m_sensorData->acc.z, m_sensorData->temperature);
        }
            break;
        case 0x52:
        {
            m_sensorData->gyro.x = (float)m_sensorRaw.stcGyro.w[0] / 2048 * 125; // 32768*2000
            m_sensorData->gyro.y = (float)m_sensorRaw.stcGyro.w[1] / 2048 * 125;
            m_sensorData->gyro.z = (float)m_sensorRaw.stcGyro.w[2] / 2048 * 125;
            //printf("gyro %0.2f %0.2f %0.2f\n", m_sensorData->gyro.x, m_sensorData->gyro.y, m_sensorData->gyro.z);
        }
            break;
        case 0x53:
        {
            m_sensorData->roll = (((m_sensorRaw.stcAngle.angle[1]<<8)|(m_sensorRaw.stcAngle.angle[0])) / 32768.0f*180); // 32768*180;
            m_sensorData->pitch= (((m_sensorRaw.stcAngle.angle[3]<<8)|(m_sensorRaw.stcAngle.angle[2])) / 32768.0f*180);
            m_sensorData->yaw  = (((m_sensorRaw.stcAngle.angle[5]<<8)|(m_sensorRaw.stcAngle.angle[4])) / 32768.0f*180);
            //printf("angle %0.2f %0.2f %0.2f\n", m_sensorData->roll, m_sensorData->pitch, m_sensorData->yaw);
        }
            break;
        case 0x54:
        {
            m_sensorData->mag.x = m_sensorRaw.stcMag.h[0];
            m_sensorData->mag.y = m_sensorRaw.stcMag.h[1];
            m_sensorData->mag.z = m_sensorRaw.stcMag.h[2];
            //printf("mag %d %d %d\n", m_sensorData->mag.x, m_sensorData->mag.y, m_sensorData->mag.z);
        }
            break;
        case 0x56: // 气压值
        {
            m_sensorData->pressure = m_sensorRaw.stcPress.lPressure;
            m_sensorData->altitude = m_sensorRaw.stcPress.lAltitude;
            //printf("mag %d %d %d\n", m_sensorData->mag.x, m_sensorData->mag.y, m_sensorData->mag.z);
        }
            break;
        default:
            break;
    }
}

void JY901::inputData(uint8_t data) noexcept
{
    static uint8_t rxCheck = 0;        // 尾校验字
    static uint8_t rxCount = 0;        // 接收计数

    m_rxBuffer[rxCount++] = data; // 将收到的数据存入缓冲区中

    if (m_rxBuffer[0] != 0x55)
    {
        // 数据头不对，则重新开始寻找0x55数据头
        rxCount = 0; // 清空缓存区
        return;
    }
    if (rxCount < JY901_PACKET_LENGTH)
        return; // 数据不满11个，则返回

    /*********** 只有接收满11个字节数据 才会进入以下程序 ************/
    for (int i = 0; i < JY901_PACKET_LENGTH - 1; i++)
        rxCheck += m_rxBuffer[i]; //校验位累加

    if (rxCheck == m_rxBuffer[JY901_PACKET_LENGTH - 1]) // 判断数据包校验是否正确
    {

        // 判断数据是哪种数据，然后将其拷贝到对应的结构体中，有些数据包需要通过上位机打开对应的输出后，才能接收到这个数据包的数据
        switch (m_rxBuffer[1])
        {
            case 0x50: // 拷贝数据 舍去包头与数据长度位
                memcpy(&m_sensorRaw.stcTime, &m_rxBuffer[2], 8);
                break;
            case 0x51:
                memcpy(&m_sensorRaw.stcAcc, &m_rxBuffer[2], 8);
                break;
            case 0x52:
                memcpy(&m_sensorRaw.stcGyro, &m_rxBuffer[2], 8);
                break;
            case 0x53:
                memcpy(&m_sensorRaw.stcAngle, &m_rxBuffer[2], 8);
                m_sensorRaw.stcAngle.angle[0] =  m_rxBuffer[2];
                m_sensorRaw.stcAngle.angle[1] =  m_rxBuffer[3];
                m_sensorRaw.stcAngle.angle[2] =  m_rxBuffer[4];
                m_sensorRaw.stcAngle.angle[3] =  m_rxBuffer[5];
                m_sensorRaw.stcAngle.angle[4] =  m_rxBuffer[6];
                m_sensorRaw.stcAngle.angle[5] =  m_rxBuffer[7];
                break;
            case 0x54:
                memcpy(&m_sensorRaw.stcMag, &m_rxBuffer[2], 8);
                break;
            case 0x55:
                memcpy(&m_sensorRaw.stcDStatus, &m_rxBuffer[2], 8);
                break;
            case 0x56:
                memcpy(&m_sensorRaw.stcPress, &m_rxBuffer[2], 8);
                break;
            case 0x57:
                memcpy(&m_sensorRaw.stcLonLat, &m_rxBuffer[2], 8);
                break;
            case 0x58:
                memcpy(&m_sensorRaw.stcGPSV, &m_rxBuffer[2], 8);
                break;
            case 0x59:
                memcpy(&m_sensorRaw.stcQ, &m_rxBuffer[2], 8);
                break;
        }
        /* JY901 数据转换 */
        rawToData();
    }
    rxCount = 0; // 清空缓存区
    rxCheck = 0; // 校验位清零
}

const jy901_t& JY901::getData() const noexcept
{
    return m_sensorData;
}

int JY901::getFd() const noexcept
{
    return m_serialFd;
}
