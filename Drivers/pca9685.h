//
// Created by fxf on 22-11-22.
//

#ifndef _PCA9685_H
#define _PCA9685_H

#include "../User/config.h"

#define PCA9685_I2C_DEV "/dev/i2c-0" // PCA9685 使用的 I2C设备
#define PCA9685_I2C_ADDR 0x40        // 将A0-A5全部接地，则其器件地址为:0x40
#define PCA9685_EN_PIN 7             // 芯片使能引脚 -> GPIO11引脚序号为 7

#define PCA9685_OSC_CLK 25000000.0f // PCA9685晶振时钟频率Hz

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE // prescaler to program the output frequency

#define PCA9685_PIN_BASE 300
#define NUM_PINS 16

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define LEDALL_ON_L 0xFA
#define LEDALL_ON_H 0xFB
#define LEDALL_OFF_L 0xFC
#define LEDALL_OFF_H 0xFD

/* sleep 模式 */
#define NORNAL_MODE 0b0
#define SLEEP_MODE 0b1

/* restart 开关 */
#define DISABLE_RESTART 0b0
#define ENABLE_RESTART 0b1

class PCA9685
{
public:
    PCA9685(const int pinBase = PCA9685_PIN_BASE, float freq = 50);

    void setPwmFreq(float freq) noexcept;

private:
    int getRegAddress(int pin);
    void reset(int fd); //pca9685PWMReset
    void writePwmToPin(int fd, int pin, int on, int off);   //pca9685PWMWrite
    void resetToPin(int fd, int pin, int tf);  //pca9685FullOff
    void setToPin(int fd, int pin, int tf);  //pca9685FullOn
    void myPwmWrite(struct wiringPiNodeStruct *node, int pin, int value);
    void myDigitalWrite(struct wiringPiNodeStruct *node, int pin, int value);
};

#endif //_PCA9685_H
