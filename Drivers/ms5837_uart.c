#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>

#include "User/log.h"

#include "ms5837_uart.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    int fd;
    float temperature;
    float depth;
} ms5837_uart_t;

/**
 * @brief  ms5837 解算板数据转换
 * @param  data 串口数据
 */
void ms5837_usart_data(void * restrict handler, uint8_t data)
{
    static uint8_t rxBuffer[20] = {0}; // 数据包
    static uint8_t rxdate[20];
    static uint8_t rxdate_count;
    static uint8_t rxCheck = 0;        // 尾校验字
    static uint8_t rxCount = 0;        // 接收计数
    static uint8_t location[5];       //D标志位坐标
    static uint8_t location_count;  
    static uint8_t location_diffren_termpe;       //标志位坐标差
    static uint8_t location_diffren_depth;       //标志位坐标差
    static uint8_t arrey_conut;
    static uint8_t MS5837_PACKET_LENGTH;
    ms5837_uart_t *temp_ptr = (ms5837_uart_t *)handler;

    rxBuffer[rxCount++] = data; // 将收到的数据存入缓冲区中
    if (rxBuffer[0] != 'T')
    {
        // 数据头不对，则重新开始寻找'T'数据头
        rxCount = 0; // 清空长度缓存
        return;
    }

     if (rxBuffer[rxCount-1] != '\n')
    {
        //还没收集到数据尾，展时不处理
        return;
    }
    MS5837_PACKET_LENGTH = rxCount;
    
    /*-----------------t提取字符串中的数字和 标记小数点的位子------*/
    for (int i = 0; i < MS5837_PACKET_LENGTH; i++)
    {
        if (isdigit(rxBuffer[i]))                   //若为数字 存入数据数组中
        {
            rxdate[rxdate_count++] = rxBuffer[i]-48;
        }
        if (rxBuffer[i] == '.'|| rxBuffer[i] == '=') //若为小数点 标记两个小数点的位子 计算差值 用于下列
        {
            location[location_count++] = i;
        }    
    }
    //for (int i = 0; i < MS5837_PACKET_LENGTH; i++)
    //{
    //    printf("rebuff[] = %c " , rxBuffer[i]);
    //}
    //printf("\r\n");
    //计算标志位差
    location_diffren_termpe = location[1] - location[0] ;
    location_diffren_depth  = location[3] - location[2] ;
    /*--------------------计算温度------------------------------*/
    if (location_diffren_termpe == 2)      //如果小数点前只有一位数据 
    {
        temp_ptr -> temperature = rxdate[arrey_conut++] + rxdate[arrey_conut++]*0.1f+rxdate[arrey_conut++]*0.01f;
    }
    else if (location_diffren_termpe == 3) //如果小数点前有i两位位数据
    {   
        if (rxBuffer[location[0]+1] == '-')//如果是负号
        {
            temp_ptr -> temperature = -(rxdate[arrey_conut++]+rxdate[arrey_conut++]*0.1f+rxdate[arrey_conut++]*0.01f);
        }
        else temp_ptr -> temperature = 10*rxdate[arrey_conut++]+rxdate[arrey_conut++]+rxdate[arrey_conut++]*0.1f+rxdate[arrey_conut++]*0.01f;
    }
    else if (location_diffren_termpe == 4) //如果小数点前有i三位位数据
    {
        temp_ptr -> temperature = 100*rxdate[arrey_conut++]+10*rxdate[arrey_conut++]+rxdate[arrey_conut++]+rxdate[arrey_conut++]*0.1f+rxdate[arrey_conut++]*0.01f;
    }
    if (location_diffren_depth == 2)
    {
        temp_ptr -> depth = rxdate[arrey_conut++]+rxdate[arrey_conut++]*0.1f+rxdate[arrey_conut++]*0.01f;
    }
    else if (location_diffren_depth == 3)
    {
        
        if (rxBuffer[location[2]+1] == '-')
        {
            temp_ptr -> depth = -(rxdate[arrey_conut++]+rxdate[arrey_conut++]*0.1f+rxdate[arrey_conut++]*0.01f);
        }
        else temp_ptr -> depth = 10*rxdate[arrey_conut++]+rxdate[arrey_conut++]+rxdate[arrey_conut++]*0.1f+rxdate[arrey_conut++]*0.01f;
  
    }

    arrey_conut = 0;
    location_count  = 0;
    rxdate_count = 0;
    rxCount = 0; // 清空缓存区
    rxCheck = 1; // 校验位清零
}

/**
 * @brief  初始化并设置 ms5837 串口
 * @param 
 *  int pinBase  pinBase > 64
 */
void ms5837_usart_init(void * restrict handler)
{
    ms5837_uart_t *temp_ptr = NULL;
    handler = calloc(1, sizeof(ms5837_uart_t));
    if (handler != NULL)
    {
        temp_ptr = (ms5837_uart_t *)handler;
        temp_ptr->fd = serialOpen(MS5837_UART_DEV, UART_BAUD_115200); // 打开串口1 波特率115200
        if (temp_ptr->fd < 0)
        {
            DRIVER_LOG_ERROR("Unable to get the fd");
        }
    }
}

void ms5837_usart_deinit(void * restrict handler)
{
    ms5837_uart_t *temp_ptr = NULL;
    temp_ptr = (ms5837_uart_t *)handler;
    close(temp_ptr -> fd);
    free(handler);
}

#ifdef __cplusplus
}
#endif
