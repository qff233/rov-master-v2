//
// Created by fxf on 22-11-22.
//

#ifndef __DRIVERS_PCA9685_H__
#define __DRIVERS_PCA9685_H__

#include "User/config.h"

#define PCA9685_PIN_BASE 300

class PCA9685
{
public:
    PCA9685(const int pinBase = PCA9685_PIN_BASE, float freq = 50);

    void setPwmFreq(float freq) noexcept;
private:
    int m_fd;

    static void PwmWriteCallBack(wiringPiNodeStruct *node, int pin, int value) noexcept;
    static void DigitalWriteCallBack(wiringPiNodeStruct *node, int pin, int value) noexcept;
    static void ResetAll(int fd) noexcept;
    static void SetPin(int fd, int pin, int tf) noexcept;
    static void ResetPin(int fd, int pin, int tf) noexcept;
    static void WritePwmToPin(int fd, int pin, int on, int off) noexcept;
    static int GetRegAddress(int pin) noexcept;
};

#endif //__DRIVERS_PCA9685_H__