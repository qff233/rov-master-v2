#include <regex>
#include <cmath>
#include <cctype> // isdigit)
#include <wiringPi.h>
#include <wiringSerial.h>

#include "ms5837.h"
#include "User/log.h"

#define MS5837_UART_DEV "/dev/ttyS1"
#define MS5837_UART_BAUD 115200

#define MS5837_PACKET_LENGTH 11
#define MS5837_CMD_LENGTH 5

using namespace std;

MS5837::MS5837()
{
    m_serialFd = serialOpen(MS5837_UART_DEV, MS5837_UART_BAUD);
    if (m_serialFd < 0)
    {
        DRIVER_LOG_ERROR("Unable to get the fd");
        return;
    }
    m_rxBuffer.resize(20); // 后面要加初始化标志位

    m_sensorData.depth = NAN;
    m_sensorData.temperature = NAN;
}

MS5837::~MS5837()
{
    close(m_serialFd);
}

bool MS5837::isValid() const noexcept
{
    return !std::isnan(m_sensorData.depth);
}

void MS5837::rawToData() noexcept
{
    static const regex tempr{"T=(-?)([[:d:]]+).([[:d:]]+)D=(-?)([[:d:]]+).([[:d:]]+)(?:[[:s:]]|.)*"};

    if (smatch m; regex_match(m_rxBuffer, m, tempr))
    {
        int temp{stoi(m[2])};
        int tempdot{stoi(m[3])};
        int depth{stoi(m[5])};
        int depthdot{stoi(m[6])};
        m_sensorData.temperature = temp + tempdot * 0.01f;
        m_sensorData.depth = depth + depthdot * 0.01f;
        if (m[1].str().find('-') != string::npos) m_sensorData.temperature *= -1;
        if (m[4].str().find('-') != string::npos) m_sensorData.depth *= -1;
    } else
    {
        DRIVER_LOG_ERROR("接受数格式化据错误 ", m_rxBuffer);
    }
}

int MS5837::inputData(uint8_t data) noexcept
{
    static uint8_t rxCount = 0; // 接收计数
    m_rxBuffer[rxCount++] = data; // 将收到的数据存入缓冲区中
    if (m_rxBuffer[0] != 'T')
    {
        // 数据头不对，则重新开始寻找'T'数据头
        rxCount = 0; // 清空长度缓存
        return -1; //一般头不会不是T 返回-1表示错误
    }
    if (m_rxBuffer[rxCount - 1] != '\n')
    {
        // 还没收集到数据尾，展时不处理
        return 1; //表示写入数据为1字节
    }
    rawToData();
    m_rxBuffer[rxCount-1] = ' ';  // 实际上string[rxCount-1]并没有在clear的时候清除 手动清除防止只读前几位就送入格式化
    rxCount = 0; // 清空缓存区
    return 0; // 写入数据为0字节 告诉外面跳出循环
}

const Ms5837Data &MS5837::getData() const noexcept
{
    return m_sensorData;
}

int MS5837::getFd() const noexcept
{
    return m_serialFd;
}

float MS5837::getTemperature() const noexcept {
    return m_sensorData.temperature;
}

float MS5837::getDepth() const noexcept {
    return m_sensorData.depth;
}
