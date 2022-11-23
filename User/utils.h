//
// Created by fxf on 22-11-23.
//

#ifndef _UTILS_H
#define _UTILS_H

#define SMOOTH_LENTH 1

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt))) //限幅函数
#define Rad2Deg(rad) (rad * 180.0f / PI) // 弧度制转角度值
#define Deg2Rad(deg) (deg * PI / 180.0f) // 角度值转弧度制

uint32_t bubble_filter(uint32_t *value);
float kalman_filter(float *Original_Data);
float value_smooth(float data);

#endif //_UTILS_H
