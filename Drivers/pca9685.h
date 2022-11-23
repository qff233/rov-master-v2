//未完成........................
//
// Created by fxf on 22-11-22.
//

#ifndef __DRIVERS_PCA9685_H__
#define __DRIVERS_PCA9685_H__

#include "User/config.h"

class PCA9685
{
public:
    PCA9685(const int pinBase = PCA9685_PIN_BASE, float freq = 50);

    void setPwmFreq(float freq) noexcept;
private:
    static int GetRegAddress(int pin) noexcept;
    static void Reset(int fd) noexcept; //pca9685PWMReset
    static void WritePwmToPin(int fd, int pin, int on, int off) noexcept;   //pca9685PWMWrite
    static void ResetPin(int fd, int pin, int tf) noexcept;  //pca9685FullOff
    static void SetPin(int fd, int pin, int tf) noexcept;  //pca9685FullOn
    static void PwmWriteCallBack(struct wiringPiNodeStruct *node, int pin, int value) noexcept;
    static void DigitalWriteCallBack(struct wiringPiNodeStruct *node, int pin, int value) noexcept;

    int m_fd;
};

#endif //__DRIVERS_PCA9685_H__
