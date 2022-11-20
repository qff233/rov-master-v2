//
// Created by fxf on 22-11-19.
//

#ifndef _JY901_H
#define _JY901_H

#include "../user/config.h"

#define JY901_UART_DEV "/dev/ttyS2" // JY901 UART 号
#define JY901_UART_BAUD 9600         // JY901 UART 波特率

#define JY901_PACKET_LENGTH 11   // JY901 数据包长度
#define JY901_CMD_LENGTH 5       // JY901 命令长度

/* 时间 */
struct STime
{
    unsigned char ucYear;
    unsigned char ucMonth;
    unsigned char ucDay;
    unsigned char ucHour;
    unsigned char ucMinute;
    unsigned char ucSecond;
    unsigned short usMiliSecond;
};

/* 加速度 */
struct SAcc
{
    short a[3];
    short T;
};

/* 角速度 */
struct SGyro
{
    short w[3];
    short T;
};

/* 角度 */
struct SAngle
{
    short angle[6];
    short T;
};

/* 磁场 */
struct SMag
{
    short h[3];
    short T;
};

/* 连接状态 */
struct SDStatus
{
    short sDStatus[4];
};

/* 气压/海拔 */
struct SPress
{
    long lPressure;
    long lAltitude;
};

/* 经纬度 */
struct SLonLat
{
    long lLon;
    long lLat;
};

/* GPS */
struct SGPSV
{
    short sGPSHeight;
    short sGPSYaw;
    long lGPSVelocity;
};

/* 四元素 */
struct SQ
{
    short q[4];
};

/* JY901 原始数据 */
struct jy901_raw_t
{
    STime stcTime;
    SAcc stcAcc;
    SGyro stcGyro;
    SAngle stcAngle;
    SMag stcMag;
    SDStatus stcDStatus;
    SPress stcPress;
    SLonLat stcLonLat;
    SGPSV stcGPSV;
    SQ stcQ;
};

//x、y、z 三维浮点
struct Vector3f
{
    float x;
    float y;
    float z;

};

//x、y、z 三维整形
struct Vector3s
{
    short x;
    short y;
    short z;

};

/* JY901 真实数据 */
struct jy901_t
{
    Vector3f acc;     //加速度
    Vector3f gyro;    //角速度
    Vector3f speed;   //地速
    Vector3s mag;     //磁场
    float pressure;   // 大气压
    float altitude;   // 海拔高度
    float roll;        //横滚角
    float pitch;       //俯仰角
    float yaw;         //航向角
    float temperature; //温度

};

class JY901
{
public:
    //定义构造与析构函数
    JY901(); //传入自定义的设备号和串口波特率

    //定义成员函数
    void inputData(uint8_t data) noexcept;   //传入一个字节的原始串口数据包
    const jy901_t& getData() const noexcept; //读取jy901的数据
    int getFd() const noexcept;  //获取fd

private:
    jy901_t m_sensorData;    //传感器数据
    jy901_raw_t m_sensorRaw; //传感器原始数据
    uint8_t m_rxBuffer[JY901_PACKET_LENGTH + 1] = {0};   //接收缓存区
    int m_serialFd;  //串口获取到的描述值

    const uint8_t m_JY901_RESET_CMD[JY901_CMD_LENGTH] = {0xFF, 0xAA, 0x00, 0x01, 0x00}; // 0x00-设置保存  0x01-恢复出厂设置并保存

    void rawToData() noexcept;   //把raw数据转换为data数据
};

#endif //_JY901_H
