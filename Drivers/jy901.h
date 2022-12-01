#ifndef __DRIVERS_JY901_H__
#define __DRIVERS_JY901_H__

#include "User/config.h"

#define JY901_PACKET_LENGTH 11   // JY901 数据包长度
#define JY901_CMD_LENGTH 5       // JY901 命令长度


/* 时间 */
struct STime
{
    uint8_t ucYear;
    uint8_t ucMonth;
    uint8_t ucDay;
    uint8_t ucHour;
    uint8_t ucMinute;
    uint8_t ucSecond;
    uint16_t usMiliSecond;
};

/* 加速度 */
struct SAcc
{
    int16_t a[3];
    int16_t T;
};

/* 角速度 */
struct SGyro
{
    int16_t w[3];
    int16_t T;
};

/* 角度 */
struct SAngle
{
    int16_t angle[6];
    int16_t T;
};

/* 磁场 */
struct SMag
{
    int16_t h[3];
    int16_t T;
};

/* 连接状态 */
struct SDStatus
{
    int16_t sDStatus[4];
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
    int16_t sGPSHeight;
    int16_t sGPSYaw;
    long lGPSVelocity;
};

/* 四元素 */
struct SQ
{
    int16_t q[4];
};

/* JY901 原始数据 */
struct Jy901Raw
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
    int16_t x;
    int16_t y;
    int16_t z;

};

/* JY901 真实数据 */
struct Jy901Data
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
    int inputData(uint8_t data) noexcept;   //传入一个字节的原始串口数据包
    void reset() const noexcept;

    bool isValid() const noexcept;

    const Jy901Data& getData() const noexcept; //读取jy901的数据
    int getFd() const noexcept;  //获取fd
    float getYaw() const noexcept;
    float getRoll() const noexcept;
    float getPitch() const noexcept;
    float getXGyro() const noexcept;
    float getYGyro() const noexcept;
    float getZGyro() const noexcept;
private:
    Jy901Data m_sensorData;    //传感器数据
    Jy901Raw m_sensorRaw; //传感器原始数据
    uint8_t m_rxBuffer[JY901_PACKET_LENGTH + 1] = {0};   //接收缓存区
    int m_serialFd;  //串口获取到的描述值

    const char m_JY901_RESET_CMD[JY901_CMD_LENGTH] = {0xFF, 0xAA, 0x00, 0x01, 0x00}; // 0x00-设置保存  0x01-恢复出厂设置并保存

    void rawToData() noexcept;   //把raw数据转换为data数据
};

#endif //__DRIVERS_JY901_H__
