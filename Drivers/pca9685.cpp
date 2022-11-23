//
// Created by fxf on 22-11-22.
//

#define LOG_TAG "PCA9685"
#define LOG_LVL ELOG_LVL_INFO

#include <elog.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "User/utils.h"

#include "pca9685.h"

#define PCA9685_I2C_DEV "/dev/i2c-0" // PCA9685 使用的 I2C设备
#define PCA9685_I2C_ADDR 0x40        // 将A0-A5全部接地，则其器件地址为:0x40
#define PCA9685_EN_PIN 7             // 芯片使能引脚 -> GPIO11引脚序号为 7

#define PCA9685_OSC_CLK 25000000.0f // PCA9685晶振时钟频率Hz

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE // prescaler to program the output frequency

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

#define SET_BIT(x) (1 << x)         // |
#define RESET_BIT(x) (~(1 << x))    // &

PCA9685::PCA9685(const int pinBase, float freq) noexcept
{
int prev_settings;  //读取到的之前的寄存器值
struct wiringPiNodeStruct *node = NULL; // 指针初始化为NULL，以免产生段错误

// PCA9685_EN_PIN，即 GPIOG11 低电平使能
pinMode(PCA9685_EN_PIN, OUTPUT);
digitalWrite(PCA9685_EN_PIN, LOW);

// 小于0代表无法找到该i2c接口，输入命令 sudo npi-config 使能该i2c接口
m_fd = wiringPiI2CSetupInterface(PCA9685_I2C_DEV, PCA9685_I2C_ADDR);
if (m_fd < 0)
{
log_e("Cannot init i2c dev");
return;
}

/* 检测是否存在 pca9685 器件
 * 小于0代表读取失败，代表不存在该 PCA9685 器件，或者器件地址错误
 */
if ((prev_settings = wiringPiI2CReadReg8(m_fd, PCA9685_MODE1)) < 0)
{
log_e("Dev no detect");
return;
}

wiringPiI2CWriteReg8(m_fd, PCA9685_MODE1, (prev_settings | SET_BIT(5)) & RESET_BIT(7));

if (freq < 40 || freq > 1000)
{
log_w("Freq range should be in [40, 1000]");
freq = constrain(freq,40,1000);
}

// 设置 PWM 频率，启动输出
setPwmFreq(freq);

// 创建节点 16 pins [0..15] + [16] for all
node = wiringPiNewNode(pinBase, NUM_PINS + 1);
if (!node)
{
log_e("Cannot create new wiringPiNode");
return;
}

// 注册方法
node->fd = m_fd;
node->pwmWrite = PwmWriteCallBack;
node->digitalWrite = DigitalWriteCallBack;

// 重置所有输出
Reset(m_fd);

//后面要加初始化标志位
}

void PCA9685::setPwmFreq(float freq) noexcept
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
int settings = wiringPiI2CReadReg8(m_fd, PCA9685_MODE1) & RESET_BIT(7); // Set restart bit to 0
int sleep = settings | SET_BIT(4);                                  // Set sleep bit to 1
int wake = settings & RESET_BIT(4);                                   // Set sleep bit to 0
int restart = wake | SET_BIT(7);                                    // Set restart bit to 1

// Go to sleep, set prescale and wake up again.
wiringPiI2CWriteReg8(m_fd, PCA9685_MODE1, sleep);
wiringPiI2CWriteReg8(m_fd, PCA9685_PRESCALE, prescale);
wiringPiI2CWriteReg8(m_fd, PCA9685_MODE1, wake);

// Now wait a millisecond until oscillator finished stabilizing and restart PWM.
delay(1);
wiringPiI2CWriteReg8(m_fd, PCA9685_MODE1, restart);
}

void PCA9685::PwmWriteCallBack(wiringPiNodeStruct *node, int pin, int value) noexcept
{
int fd = node->fd;
int ipin = pin - node->pinBase;

value = constrain(value,0,4096);
WritePwmToPin(fd, ipin, 0, value);
}

void PCA9685::DigitalWriteCallBack(wiringPiNodeStruct *node, int pin, int value) noexcept
{
int fd = node->fd;
int ipin = pin - node->pinBase;

if (value) SetPin(fd, ipin, 1);
else ResetPin(fd, ipin, 1);
}

void PCA9685::SetPin(int fd, int pin, int tf) noexcept
{
int reg = GetRegAddress(pin) + 1; // LEDX_ON_H 寄存器
int state = wiringPiI2CReadReg8(fd, reg);

// 根据 tf 设置 第4bit 为 0 or 1
state = tf ? (state | SET_BIT(4)) : (state & RESET_BIT(4));

wiringPiI2CWriteReg8(fd, reg, state);

//  full-off 优先级高于 full-on (datasheet P23)
if (tf) ResetPin(fd, pin, 0);
}

void PCA9685::ResetPin(int fd, int pin, int tf) noexcept
{
int reg = GetRegAddress(pin) + 3; // LEDX_OFF_H 寄存器
int state = wiringPiI2CReadReg8(fd, reg);

// 根据 tf 设置 第4bit 为 0 or 1
state = tf ? (state | SET_BIT(4)) : (state & RESET_BIT(4));

wiringPiI2CWriteReg8(fd, reg, state);
}

void PCA9685::WritePwmToPin(int fd, int pin, int on, int off) noexcept
{
int reg = GetRegAddress(pin);

// 可写入位 12bit，最大值为 4095    on + off = 4095
wiringPiI2CWriteReg16(fd, reg, on & 0x0FFF);
wiringPiI2CWriteReg16(fd, reg + 2, off & 0x0FFF);
}

int PCA9685::GetRegAddress(int pin) noexcept
{
// 计算获得对应引脚寄存器地址 (见datasheet P9)
return (pin >= NUM_PINS ? LEDALL_ON_L : LED0_ON_L + 4 * pin);
}
