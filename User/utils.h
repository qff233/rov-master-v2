#ifndef __USER_UTILS_H__
#define __USER_UTILS_H__

#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>
#include <string>
#include <string_view>

#define SMOOTH_LENGTH 1

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt))) //限幅函数
#define rad2Deg(rad) (rad * 180.0f / M_PI) // 弧度制转角度值
#define deg2Rad(deg) (deg * M_PI / 180.0f) // 角度值转弧度制

uint32_t bubble_filter(uint32_t *value);

class KalmanFilter 
{
public:
    KalmanFilter(float prevData = 0, float p = 10, float q = 0.0001, float r = 0.001,float kGain = 0);
    float operator()(float Original_Data);
private:
    float prevData;
    float p;
    float q;
    float r = 0.001;
    float kGain = 0;
};

class SmoothFilter 
{
public:
    float operator()(float data);
private:
    float data_sum;
    float data_ave;
    uint8_t count;
    float depth_last;
};

std::vector<std::string> split(std::string_view str, std::string_view delim);

template<class T>
class Global 
{
public:
    static T* Get() { return *GetPPtr(); }
    
    template<typename SubType, typename... Args>
    static void New(Args&&... args) 
    { 
        assert(Get() == nullptr);
        *GetPPtr() = new SubType(std::forward<Args>(args)...);
    }

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
