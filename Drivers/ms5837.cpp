#define LOG_TAG "MS5837"
#define LOG_LVL ELOG_LVL_INFO

#include <elog.h>   // log_e()
#include <cctype>   // isdigit()
#include <wiringPi.h>
#include <wiringSerial.h>

#include "ms5837.h"

using namespace std;

MS5837::MS5837()
{
    m_serialFd = serialOpen(MS5837_UART_DEV, MS5837_UART_BAUD);
    if (m_serialFd < 0)
        log_e("Unable to get the fd");
}

void MS5837::rawToData(uint8_t packet_length) noexcept
{
    static uint8_t rxDate_count;
    static uint8_t rxCheck = 0;        // 尾校验字
    static uint8_t location[5];       //D标志位坐标
    static uint8_t location_count;
    static uint8_t location_diffren_termpe;       //标志位坐标差
    static uint8_t location_diffren_depth;       //标志位坐标差
    static uint8_t arrey_conut;
/*-----------------t提取字符串中的数字和 标记小数点的位子------*/
    for (int i = 0; i < packet_length; i++)
    {
        if (isdigit(m_rxBuffer[i]))                   //若为数字 存入数据数组中
        {
            m_rxData[rxDate_count++] = m_rxBuffer[i]-48;
        }
        if (m_rxBuffer[i] == '.'|| m_rxBuffer[i] == '=') //若为小数点 标记两个小数点的位子 计算差值 用于下列
        {
            location[location_count++] = i;
        }
    }
    location_diffren_termpe = location[1] - location[0] ;
    location_diffren_depth  = location[3] - location[2] ;
    /*--------------------计算温度------------------------------*/
    if (location_diffren_termpe == 2)      //如果小数点前只有一位数据
    {
        m_sensorData.temperature = m_rxData[arrey_conut++] + m_rxData[arrey_conut++]*0.1f+m_rxData[arrey_conut++]*0.01f;
    }
    else if (location_diffren_termpe == 3) //如果小数点前有i两位位数据
    {
        if (m_rxBuffer[location[0]+1] == '-')//如果是负号
        {
            m_sensorData.temperature = -(m_rxData[arrey_conut++]+m_rxData[arrey_conut++]*0.1f+m_rxData[arrey_conut++]*0.01f);
        }
        else m_sensorData.temperature = 10*m_rxData[arrey_conut++]+m_rxData[arrey_conut++]+m_rxData[arrey_conut++]*0.1f+m_rxData[arrey_conut++]*0.01f;
    }
    else if (location_diffren_termpe == 4) //如果小数点前有i三位位数据
    {
        m_sensorData.temperature = 100*m_rxData[arrey_conut++]+10*m_rxData[arrey_conut++]+m_rxData[arrey_conut++]+m_rxData[arrey_conut++]*0.1f+m_rxData[arrey_conut++]*0.01f;
    }
    if (location_diffren_depth == 2)
    {
        m_sensorData.depth = m_rxData[arrey_conut++]+m_rxData[arrey_conut++]*0.1f+m_rxData[arrey_conut++]*0.01f;
    }
    else if (location_diffren_depth == 3)
    {

        if (m_rxBuffer[location[2]+1] == '-')
        {
            m_sensorData.depth = -(m_rxData[arrey_conut++]+m_rxData[arrey_conut++]*0.1f+m_rxData[arrey_conut++]*0.01f);
        }
        else m_sensorData.depth = 10*m_rxData[arrey_conut++]+m_rxData[arrey_conut++]+m_rxData[arrey_conut++]*0.1f+m_rxData[arrey_conut++]*0.01f;

    }

    arrey_conut = 0;
    location_count  = 0;
    rxDate_count = 0;
    rxCheck = 1; // 校验位清零
}

void MS5837::inputData(uint8_t data) noexcept
{
    static uint8_t rxCount = 0;        // 接收计数
    static uint8_t ms5837_packet_length;

    m_rxBuffer[rxCount++] = data; // 将收到的数据存入缓冲区中
    if (m_rxBuffer[0] != 'T')
    {
    // 数据头不对，则重新开始寻找'T'数据头
    rxCount = 0; // 清空长度缓存
    return;
    }

    if (m_rxBuffer[rxCount-1] != '\n')
    {
        //还没收集到数据尾，展时不处理
        return;
    }
    ms5837_packet_length = rxCount;
    rawToData(ms5837_packet_length);
    rxCount = 0; // 清空缓存区
}

const ms5837_data& MS5837::getData() const noexcept
{
    return m_sensorData;
}

int MS5837::getFd() const noexcept
{
    return m_serialFd;
}
