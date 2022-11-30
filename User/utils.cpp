//
// Created by fxf on 22-11-23.
//

#include "utils.h"

#include <cmath>

std::vector<std::string> split(std::string_view str, std::string_view delim)
{
    std::vector<std::string> results;
    size_t begin = 0;
    size_t pos = str.find(delim);
    while (pos < str.length())
    {
        size_t length = pos - begin;
        results.emplace_back(str.substr(begin, length));
        begin = pos + delim.length();
        pos = str.find(delim, begin);
    }
    if(str.length()-begin)
        results.emplace_back(str.substr(begin, str.length()-begin));
    return results;
}

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
KalmanFilter::KalmanFilter(float prevData, float p, float q, float r,float kGain)
    : prevData(prevData),
      p(p),
      q(q),
      r(r),
      kGain(kGain)
{
}

float KalmanFilter::operator()(float original_data)
{
    static float prevData = 0;
    static float p = 10, q = 0.0001, r = 0.001, kGain = 0;

    p = p + q;
    kGain = p / (p + r);

    original_data = prevData + (kGain * (original_data - prevData));
    p = (1 - kGain) * p;

    prevData = original_data;
    return original_data;
}

float SmoothFilter::operator()(float data) 
{
    if (fabs(depth_last - data)> 1)
    {
        data_sum = 0;
        count = 0;
        return depth_last;       //数据异常 不进行下列计算
    }
    /* ---------------------均值滤波--------------------------- */
    if (count == SMOOTH_LENGTH)
    {
        data_sum += data ;
        data_ave = 0;
        data_ave = data_sum / (SMOOTH_LENGTH + 1);
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
