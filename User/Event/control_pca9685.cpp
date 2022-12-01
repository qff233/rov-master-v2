#include "control_pca9685.h"
#include <wiringPi.h>

#include <pca9685.h>
#include "User/utils.h"

/**
 * @brief  计算 PWM 高电平所占的周期
 * @param  duty 高电平持续的时间，单位us。
 *  eg. duty = 1500us = 1.5ms
 *  高电平所占的周期 = 1.5 / 20 * 4096
 */

void EventPCA9685::process() noexcept
{
    static int pinBase = Global<PCA9685>::Get()->getPinBase();
    const int16_t* data = Global<Control>::Get()->get6RawData();
    for(size_t i = 0; i < 6; ++i)
        ::pwmWrite(pinBase + i, PCA9685::CalcTicks(data[i]));
    
    auto devices = Global<Control>::Get()->getPwmDevices();
    for(const auto& i : devices) 
    {
        ::pwmWrite(pinBase + i.m_channel, PCA9685::CalcTicks(i.m_params.cur));
    }
}
