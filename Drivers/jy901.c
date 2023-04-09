#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include "User/log.h"

#include "jy901.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define JY901_UART_DEV "/dev/ttyS2" // JY901 UART 号
#define JY901_UART_BAUD 9600        // JY901 UART 波特率
#define JY901_PACKET_LENGTH 11      // JY901 数据包长度
#define JY901_CMD_LENGTH 5          // JY901 命令长度

static const uint8_t JY901_RESET_CMD[JY901_CMD_LENGTH] = {0xFF, 0xAA, 0x00, 0x01, 0x00}; // 0x00-设置保存  0x01-恢复出厂设置并保存

static jy901_raw_t jy901_raw = {0}; // JY901 原始数据

void jy901_reset(void * restrict handler)
{
    jy901_t *temp_ptr = NULL;
    temp_ptr = (jy901_t)handler;
    write(temp_ptr->fd, JY901_RESET_CMD, JY901_CMD_LENGTH);
}

static void jy901_convert(uint8_t which, jy901_t * jy901)
{
    // 选择数据包
    switch (which)
    {
    case 0x51:
    {
        jy901->acc.x = (jy901_raw.stcAcc.a[0] / 32768.0f * 16); // 32768*16
        jy901->acc.y = (jy901_raw.stcAcc.a[1] / 32768.0f * 16);
        jy901->acc.z = (jy901_raw.stcAcc.a[2] / 32768.0f * 16);
        jy901->temperature = (jy901_raw.stcAcc.T / 100.0f);
    }
    break;
    case 0x52:
    {
        jy901->gyro.x = (float)jy901_raw.stcGyro.w[0] / 2048 * 125; // 32768*2000
        jy901->gyro.y = (float)jy901_raw.stcGyro.w[1] / 2048 * 125;
        jy901->gyro.z = (float)jy901_raw.stcGyro.w[2] / 2048 * 125;
    }
    break;
    case 0x53:
    {
        jy901->roll = (((jy901_raw.stcAngle.angle[1] << 8) | (jy901_raw.stcAngle.angle[0])) / 32768.0f * 180); // 32768*180;
        jy901->pitch = (((jy901_raw.stcAngle.angle[3] << 8) | (jy901_raw.stcAngle.angle[2])) / 32768.0f * 180);
        jy901->yaw = (((jy901_raw.stcAngle.angle[5] << 8) | (jy901_raw.stcAngle.angle[4])) / 32768.0f * 180);
    }
    break;
    case 0x54:
    {
        jy901->mag.x = jy901_raw.stcMag.h[0];
        jy901->mag.y = jy901_raw.stcMag.h[1];
        jy901->mag.z = jy901_raw.stcMag.h[2];
    }
    break;
    case 0x56: // 气压值
    {
        jy901->pressure = jy901_raw.stcPress.lPressure;
        jy901->altitude = jy901_raw.stcPress.lAltitude;
    }
    break;
    default:
        break;
    }
}

void jy901_input_data(void * restrict handler, uint8_t data)
{
    jy901_t *temp_ptr = NULL;
    temp_ptr = (jy901_t)handler;
    static uint8_t i;
    static uint8_t rxBuffer[20] = {0}; // 数据包
    static uint8_t rxCheck = 0;        // 尾校验字
    static uint8_t rxCount = 0;        // 接收计数

    rxBuffer[rxCount++] = data; // 将收到的数据存入缓冲区中

    if (rxBuffer[0] != 0x55)
    {
        // 数据头不对，则重新开始寻找0x55数据头
        rxCount = 0; // 清空缓存区
        return;
    }
    if (rxCount < JY901_PACKET_LENGTH)
        return; // 数据不满11个，则返回

    /*********** 只有接收满11个字节数据 才会进入以下程序 ************/
    for (i = 0; i < JY901_PACKET_LENGTH - 1; i++)
        rxCheck += rxBuffer[i]; // 校验位累加

    if (rxCheck == rxBuffer[JY901_PACKET_LENGTH - 1]) // 判断数据包校验是否正确
    {

        // 判断数据是哪种数据，然后将其拷贝到对应的结构体中，有些数据包需要通过上位机打开对应的输出后，才能接收到这个数据包的数据
        switch (rxBuffer[1])
        {
        case 0x50: // 拷贝数据 舍去包头与数据长度位
            memcpy(&jy901_raw.stcTime, &rxBuffer[2], 8);
            break;
        case 0x51:
            memcpy(&jy901_raw.stcAcc, &rxBuffer[2], 8);
            break;
        case 0x52:
            memcpy(&jy901_raw.stcGyro, &rxBuffer[2], 8);
            break;
        case 0x53:
            memcpy(&jy901_raw.stcAngle, &rxBuffer[2], 8);
            jy901_raw.stcAngle.angle[0] = rxBuffer[2];
            jy901_raw.stcAngle.angle[1] = rxBuffer[3];
            jy901_raw.stcAngle.angle[2] = rxBuffer[4];
            jy901_raw.stcAngle.angle[3] = rxBuffer[5];
            jy901_raw.stcAngle.angle[4] = rxBuffer[6];
            jy901_raw.stcAngle.angle[5] = rxBuffer[7];
            break;
        case 0x54:
            memcpy(&jy901_raw.stcMag, &rxBuffer[2], 8);
            break;
        case 0x55:
            memcpy(&jy901_raw.stcDStatus, &rxBuffer[2], 8);
            break;
        case 0x56:
            memcpy(&jy901_raw.stcPress, &rxBuffer[2], 8);
            break;
        case 0x57:
            memcpy(&jy901_raw.stcLonLat, &rxBuffer[2], 8);
            break;
        case 0x58:
            memcpy(&jy901_raw.stcGPSV, &rxBuffer[2], 8);
            break;
        case 0x59:
            memcpy(&jy901_raw.stcQ, &rxBuffer[2], 8);
            break;
        }
        /* JY901 数据转换 */
        jy901_convert(rxBuffer[1], temp_ptr);
    }
    rxCount = 0; // 清空缓存区
    rxCheck = 0; // 校验位清零
}

float jy901_get_yaw(void * restrict handler)
{
    jy901_t *temp_ptr = NULL;
    temp_ptr = (jy901_t*)handler;
    retrun (temp_ptr -> jy901.yaw);
}

float jy901_get_roll(void * restrict handler)
{
    jy901_t *temp_ptr = NULL;
    temp_ptr = (jy901_t*)handler;
    retrun (temp_ptr -> jy901.roll);
}

float jy901_get_pitch(void * restrict handler)
{
    jy901_t *temp_ptr = NULL;
    temp_ptr = (jy901_t*)handler;
    retrun (temp_ptr -> jy901.pitch);
}

void jy901_init(void * restrict handler)
{
    jy901_t *temp_ptr = NULL;
    handler = calloc(1, sizeof(jy901_t));
    if (handler != NULL)
    {
        temp_ptr = (jy901_t *)handler;
        temp_ptr->fd = serialOpen(JY901_UART_DEV, JY901_UART_BAUD);
        if (temp_ptr -> fd < 0)
        {
            DRIVER_LOG_ERROR("Unable to get the fd");
        }
    }
}

void jy901_deinit(void * restrict handler)
{
    jy901_t *temp_ptr = NULL;
    temp_ptr = (jy901_t*)handler;
    close(temp_ptr -> fd);
    free(handler);
}

#ifdef __cplusplus
}
#endif
