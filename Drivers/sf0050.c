#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>

#include "sf0050.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SF0050_UART_DEV "/dev/ttyS1"   // 深度传感器结算板 串口
#define SF0050_UART_BAUD 9600     // 深度结算板 UART

#define SF0050_COMMAND_DATA_LENGTH 6
#define SF0050_RAW_DATA_LENGTH 17
#define SF0050_HEAD_BYTE 0xAB

#define SF0050_FRI_DISTANCE_HB_NUM 4
#define SF0050_FRI_DISTANCE_LB_NUM 5
#define SF0050_PEAK_DISTANCE_HB_NUM 8
#define SF0050_PEAK_DISTANCE_LB_NUM 9
#define SF0050_LAST_DISTANCE_HB_NUM 12
#define SF0050_LAST_DISTANCE_LB_NUM 13

typedef struct
{
    int fd;
    uint32_t first_target_distance;
    uint32_t peak_target_distance;  //单位厘米，最小精度1厘米
    uint32_t last_target_distance;
} sf0050_t;

static const uint8_t SET_TIMING_MEASURE [SF0050_COMMAND_DATA_LENGTH] = {0xAA, 0xA2, 0x00, 0x00, 0x00, 0x08};   //定时测量命令
static const uint8_t SET_PERIOD_TIME [SF0050_COMMAND_DATA_LENGTH] = {0xAA, 0xA4, 0x00, 0x00, 0x05, 0x0B};

/// @brief 开启串口
/// @return 串口的fd
void sf0050_usart_init(void * restrict handler)
{
    sf0050_t *temp_ptr = NULL;
    handler = calloc(1, sizeof(sf0050_t));
    if (handler != NULL)
    {
        temp_ptr = (sf0050_t *)handler;
        temp_ptr->fd = serialOpen(SF0050_UART_DEV, SF0050_UART_BAUD);
        if (temp_ptr->fd < 0)
        {
            DRIVER_LOG_ERROR("Unable to get the fd");
        };
    }
}

void sf0050_usart_deinit(void * restrict handler)
{
    sf0050_t *temp_ptr = NULL;
    temp_ptr = (sf0050_t*)handler;
    close(temp_ptr -> fd);
    free(handler);
}

/// @brief SF0050校验数据包
/// @param raw_data SF0050的原始数据
/// @return 成功返回 TRUE ,失败返回 FALSE
static int sf0050_verify_rawData(uint8_t *raw_data)
{
    uint8_t real_code = raw_data[0];
    uint8_t verify_code = raw_data[SF0050_RAW_DATA_LENGTH - 1];
    for (uint8_t i = 1; i < SF0050_RAW_DATA_LENGTH -1; i++)
    {
        real_code ^= raw_data[i];
    }
    if(real_code == verify_code) return 1;
    return 0;
}

/// @brief SF0050解析数据包
/// @param raw_data SF0050返回的原始数据
/// @param data 处理之后的数据
static void sf0050_get_deepth(uint8_t *raw_data, sf0050_t *data)
{
    data -> first_target_distance = (uint32_t)(raw_data[SF0050_FRI_DISTANCE_HB_NUM] * 256 + raw_data[SF0050_FRI_DISTANCE_LB_NUM]);
    data -> peak_target_distance = (uint32_t)(raw_data[SF0050_PEAK_DISTANCE_HB_NUM] * 256 + raw_data[SF0050_PEAK_DISTANCE_LB_NUM]);
    data -> last_target_distance = (uint32_t)(raw_data[SF0050_LAST_DISTANCE_HB_NUM] * 256 + raw_data[SF0050_LAST_DISTANCE_LB_NUM]);
}

/// @brief SF0050接收串口数据包
/// @param data 每次接收到的数据
void sf0050_usart_data(void * restrict handler, uint8_t data)
{
    sf0050_t *temp_ptr = (sf0050_t *)handler;
    static uint8_t raw_data_buffer [SF0050_RAW_DATA_LENGTH] = {0};
    static uint8_t rxCount = 0;
    raw_data_buffer[rxCount++] = data;
    if (raw_data_buffer[0] != SF0050_HEAD_BYTE)
    {
        rxCount = 0;
        return;
    }
    if (SF0050_RAW_DATA_LENGTH == rxCount)
    {
        if (!sf0050_verify_rawData(raw_data_buffer))
        {
            rxCount = 0;
            return;
        }
        sf0050_get_deepth(raw_data_buffer, temp_ptr);
        rxCount = 0;
    }
}

uint32_t sf0050_get_peak_depth(void * restrict handler)
{
    sf0050_t *temp_ptr = NULL;
    temp_ptr = (sf0050_t*)handler;
    retrun (temp_ptr -> peak_target_distance);
}

uint32_t sf0050_get_first_depth(void * restrict handler)
{
    sf0050_t *temp_ptr = NULL;
    temp_ptr = (sf0050_t*)handler;
    retrun (temp_ptr -> first_target_distance);
}

uint32_t sf0050_get_last_depth(void * restrict handler)
{
    sf0050_t *temp_ptr = NULL;
    temp_ptr = (sf0050_t*)handler;
    retrun (temp_ptr -> last_target_distance);
}

#ifdef __cplusplus
}
#endif
