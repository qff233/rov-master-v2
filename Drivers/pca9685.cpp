//未完成........................

//
// Created by fxf on 22-11-22.
//

#define LOG_TAG "PCA9685"
#define LOG_LVL ELOG_LVL_INFO

#include <elog.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "pca9685.h"

PCA9685::PCA9685(const int pinBase, float freq)
{
    int fd;
    int settings, autoInc;
    struct wiringPiNodeStruct *node = NULL; // 指针初始化为NULL，以免产生段错误

    // PCA9685_EN_PIN，即 GPIOG11 低电平使能
    pinMode(PCA9685_EN_PIN, OUTPUT);
    digitalWrite(PCA9685_EN_PIN, LOW);

    // 小于0代表无法找到该i2c接口，输入命令 sudo npi-config 使能该i2c接口
    fd = wiringPiI2CSetupInterface(PCA9685_I2C_DEV, PCA9685_I2C_ADDR);
    if (fd < 0)
    {
        log_e("Cannot init i2c dev");
        return;
    }

    /* 检测是否存在 pca9685 器件
     * 小于0代表读取失败，代表不存在该 PCA9685 器件，或者器件地址错误
     */
    if ((settings = wiringPiI2CReadReg8(fd, PCA9685_MODE1)) < 0)
    {
        log_e("Dev no detect");
        return;
    }

    autoInc = (settings | 0x20) & 0x7F;
    wiringPiI2CWriteReg8(fd, PCA9685_MODE1, autoInc);

    if (freq < 40 || freq > 1000)
    {
        log_e("Freq range should be in [40, 1000]");
        freq = 50;
    }

    // 设置 PWM 频率，启动输出
    setPwmFreq(fd, freq);

    // 创建节点 16 pins [0..15] + [16] for all
    node = wiringPiNewNode(pinBase, NUM_PINS + 1);
    if (!node)
    {
        log_e("Cannot create new wiringPiNode");
        return;
    }

    // 注册方法
    node->fd = fd;
    node->pwmWrite = MyPwmWrite;
    node->digitalWrite = MyDigitalWrite;

    // 重置所有输出
    Reset(fd);

    //后面要加初始化标志位
}

void PCA9685::setPwmFreq(int fd, float freq) noexcept
{
    /**  MODE1 寄存器
    * Restart and set Mode1 register to our prefered mode:
    * Restart         : bit 7 = 1 - Will revert to 0 after restart
    * Internal Clock  : bit 6 = 0
    * Auto Increment  : bit 5 = 1
    * Normal Mode     : bit 4 = 0
    * SUB1 Disabled   : bit 3 = 0
    * SUB2 Disabled   : bit 2 = 0
    * SUB3 Disabled   : bit 1 = 0
    * ALLCALL Enabled : bit 0 = 1
    *
    * B10100001 == 0xA1
    */

    // 限制频率范围在 [40, 1000]
    freq = (freq > 1000 ? 1000 : (freq < 40 ? 40 : freq));

    /*
    * 设置 PWM 频率, prescale 寄存器公式如下: (datasheet Page 24)
    * prescale = round(osc_clock / (4096 * frequency)) - 1 , osc_clock = 25 MHz
    * round 为四舍五入,可以通过 +0.5 来实现
    */
    int prescale = (int)((PCA9685_OSC_CLK / (4096 * freq)+0) /(上位机.pca9685_pwm+1.0f)); // 1.034校准

    // Get settings and calc bytes for the different states.
    int settings = wiringPiI2CReadReg8(fd, PCA9685_MODE1) & 0x7F; // Set restart bit to 0
    int sleep = settings | 0x10;                                  // Set sleep bit to 1
    int wake = settings & 0xEF;                                   // Set sleep bit to 0
    int restart = wake | 0x80;                                    // Set restart bit to 1

    // Go to sleep, set prescale and wake up again.
    wiringPiI2CWriteReg8(fd, PCA9685_MODE1, sleep);
    wiringPiI2CWriteReg8(fd, PCA9685_PRESCALE, prescale);
    wiringPiI2CWriteReg8(fd, PCA9685_MODE1, wake);

    // Now wait a millisecond until oscillator finished stabilizing and restart PWM.
    delay(1);
    wiringPiI2CWriteReg8(fd, PCA9685_MODE1, restart);
}

int PCA9685::GetRegAddress(int pin)
{
    // 计算获得对应引脚寄存器地址 (见datasheet P9)
    return (pin >= NUM_PINS ? LEDALL_ON_L : LED0_ON_L + 4 * pin);
}

void PCA9685::Reset(int fd)
{
    wiringPiI2CWriteReg16(fd, LEDALL_ON_L, 0x0);        // ALL_LED full ON  失能
    wiringPiI2CWriteReg16(fd, LEDALL_ON_L + 2, 0x1000); // ALL_LED full OFF 使能
}

void PCA9685::WritePwmToPin(int fd, int pin, int on, int off)
{
    int reg = GetRegAddress(pin);

    // 可写入位 12bit，最大值为 4095    on + off = 4095
    wiringPiI2CWriteReg16(fd, reg, on & 0x0FFF);
    wiringPiI2CWriteReg16(fd, reg + 2, off & 0x0FFF);
}

void PCA9685::ResetPin(int fd, int pin, int tf)
{
    int reg = GetRegAddress(pin) + 3; // LEDX_OFF_H 寄存器
    int state = wiringPiI2CReadReg8(fd, reg);

    // 根据 tf 设置 第4bit 为 0 or 1
    state = tf ? (state | 0x10) : (state & 0xEF);

    wiringPiI2CWriteReg8(fd, reg, state);
}

void PCA9685::SetPin(int fd, int pin, int tf)
{
    int reg = GetRegAddress(pin) + 1; // LEDX_ON_H 寄存器
    int state = wiringPiI2CReadReg8(fd, reg);

    // 根据 tf 设置 第4bit 为 0 or 1
    state = tf ? (state | 0x10) : (state & 0xEF);

    wiringPiI2CWriteReg8(fd, reg, state);

    //  full-off 优先级高于 full-on (datasheet P23)
    if (tf)
        ResetPin(fd, pin, 0);
}

void PCA9685::MyPwmWrite(struct wiringPiNodeStruct *node, int pin, int value)
{
    int fd = node->fd;
    int ipin = pin - node->pinBase;

    if (value >= 4096) SetPin(fd, ipin, 1);
    else if (value > 0) WritePwmToPin(fd, ipin, 0, value);
    else ResetPin(fd, ipin, 1);
}

void PCA9685::MyDigitalWrite(struct wiringPiNodeStruct *node, int pin, int value)
{
    int fd = node->fd;
    int ipin = pin - node->pinBase;

    if (value) SetPin(fd, ipin, 1);
    else ResetPin(fd, ipin, 1);
}
