//
// Created by fxf on 22-11-23.
//

#include "utils.h"

uint32_t bubble_filter(uint32_t *value)
{
    uint8_t i, j, swapFlag; //交换标志位
    uint32_t res = 0;       //reserve 暂存
    uint32_t med = 0;       //中值

    for (j = 0; j < 10 - 1; j++)
    {
        swapFlag = 0; //每一个大循环检验
        for (i = 0; i < 9 - j; i++)
        {
            if (value[i] > value[i + 1])
            { //>升序   <降序
                res = value[i];
                value[i] = value[i + 1];
                value[i + 1] = res;

                swapFlag = 1; //若交换置1
            }
        }
        if (0 == swapFlag)
            break; //未发生交换，则提前结束
    }
    med = (*(value + 4) + *(value + 5) + *(value + 6)) / 3; //中间平均值
    return med;
}

/*
其中p的初值可以随便取，但是不能为0（为0的话卡尔曼滤波器就认为已经是最优滤波器了）
q,r的值需要我们试出来，讲白了就是(买的破温度计有多破，以及你的超人力有多强)
q参数调整滤波后的曲线与实测曲线的相近程度，q越大越接近。
r参数调滤波后的曲线平滑程度，r越大越平滑。
*/
float kalman_filter(float *Original_Data)
{
    static float prevData = 0;
    static float p = 10, q = 0.0001, r = 0.001, kGain = 0;

    p = p + q;
    kGain = p / (p + r);

    *Original_Data = prevData + (kGain * (*Original_Data - prevData));
    p = (1 - kGain) * p;

    prevData = *Original_Data;

    return *Original_Data;
}

/**
 * @brief ms5837的均值滤波
 * @param data 未经滤波的深度数据
 */
float value_smooth(float data)
{
    static float data_sum;
    static float data_ave;
    static char count;
    static float depth_mark;        //初始深度。
    static float depth_stand;       //初始深度。
    static float depth_abort;       //异常深度。
    static float depth_abort_flag;  //异常深度。
    static float depth_abort_count; //异常深度。
    static float depth_diffren;     //异常深度。
    static float depth_last;        //历史深度。
    static float ave_flag;          //历史深度
    /*-----如果当前深度比上次深度差异过大，将当前深度作为异常深度*/
    if (fabs(depth_last - data)> 1)
    {
        data_sum = 0;
        count = 0;
        return depth_last;       //数据异常 不进行下列计算
    }
    /* ---------------------均值滤波--------------------------- */
    if (count == SMOOTH_LENTH)
    {
        data_sum += data ;
        data_ave = 0;
        data_ave = data_sum / (SMOOTH_LENTH + 1);
        depth_last = data_ave;
        data_sum = data;
    }
    else
    {
        data_sum += data ;
        count++;
    }

    return data_ave;
}
