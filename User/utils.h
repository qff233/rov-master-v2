//
// Created by fxf on 22-11-23.
//

#ifndef _UTILS_H
#define _UTILS_H

#include "config.h"
#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>
#include <string_view>

#define SMOOTH_LENGTH 1

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt))) //限幅函数
#define rad2Deg(rad) (rad * 180.0f / M_PI) // 弧度制转角度值
#define deg2Rad(deg) (deg * M_PI / 180.0f) // 角度值转弧度制

uint32_t bubble_filter(uint32_t *value);
float kalman_filter(float *Original_Data);  //卡尔曼滤波
float smooth_filter(float data);             // 均值滤波

template<class T>
class Global 
{
public:
    static T* Get() { return *GetPPtr(); }
    
    template<typename... Args>
    static void New(Args&&... args) 
    { 
        assert(Get() == nullptr);
        *GetPPtr() = new T(std::forward<Args>(args)...);
    }

    static void Delete() 
    {
        if(Get() != nullptr) 
        {
            delete Get();
            *GetPPtr() = nullptr;
        }
    }
private:
    static T** GetPPtr() {
        static T* ptr = nullptr;
        return &ptr;
    }
};

#endif //_UTILS_H
