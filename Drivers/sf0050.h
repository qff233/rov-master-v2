#ifndef __DRIVERS_SF0050_H__
#define __DRIVERS_SF0050_H__

#include "User/config.h"

#define SF0050_CMD_LENGTH 6
#define SF0050_PACKET_LENGTH 17

struct SF0050Data
{
    uint32_t first_target_distance;
    uint32_t peak_target_distance;  //单位厘米，最小精度1厘米
    uint32_t last_target_distance;
};

class SF0050
{
public:
    //定义构造与析构函数
    SF0050(); //传入自定义的设备号和串口波特率

    //定义成员函数
    int inputData(uint8_t data) noexcept;   //传入一个字节的原始串口数据包

    bool isValid() const noexcept;
    bool verifyRawData();

    const SF0050Data& getData() const noexcept; //读取SF0050的数据
    int getFd() const noexcept;  //获取fd
    uint32_t getDistance() const noexcept;

private:
    SF0050Data m_sensorData;    //传感器数据
    uint8_t m_rxBuffer[SF0050_PACKET_LENGTH + 1] = {0};   //接收缓存区
    int m_serialFd;  //串口获取到的描述值

    const uint8_t SET_TIMING_MEASURE [SF0050_CMD_LENGTH] = {0xAA, 0xA2, 0x00, 0x00, 0x00, 0x08};

    void rawToData() noexcept;   //把raw数据转换为data数据
};

#endif //__DRIVERS_SF0050_H__
