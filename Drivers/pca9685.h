//
// Created by fxf on 22-11-22.
//

#ifndef __DRIVERS_PCA9685_H__
#define __DRIVERS_PCA9685_H__

#include <wiringPi.h>

class PCA9685
{
public:
    static const int HERTZ = 50;
    static const int MAX_PWM = 4096;

    static int CalcTicks(int16_t duty);

    PCA9685(const int pinBase = 300, float freq = 50);
    ~PCA9685();

    void pwmWrite(int pin, int value) noexcept;
    void setPwmFreq(float freq, float pwm_calibration = 0.0f) noexcept;
    int getPinBase();

private:
    int m_fd;
    int m_pinBase;
    wiringPiNodeStruct *m_node = nullptr;

    static void PwmWriteCallBack(wiringPiNodeStruct *node, int pin, int value) noexcept;
    static void DigitalWriteCallBack(wiringPiNodeStruct *node, int pin, int value) noexcept;
    static void ResetAll(int fd) noexcept;
    static void SetPin(int fd, int pin, int tf) noexcept;
    static void ResetPin(int fd, int pin, int tf) noexcept;
    static void WritePwmToPin(int fd, int pin, int on, int off) noexcept;
    static int GetRegAddress(int pin) noexcept;
};

#endif //__DRIVERS_PCA9685_H__