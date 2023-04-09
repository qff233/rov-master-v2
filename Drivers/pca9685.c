#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "User/log.h"

#include "pca9685.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define PCA9685_I2C_DEV "/dev/i2c-0" // PCA9685 使用的 I2C设备
#define PCA9685_I2C_ADDR 0x40        // 将A0-A5全部接地，则其器件地址为:0x40
#define PCA9685_OE_PIN 7             // 芯片使能引脚 -> GPIO11引脚序号为 7

#define PCA9685_OSC_CLK 25000000.0f // PCA9685晶振时钟频率Hz

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE // prescaler to program the output frequency

#define PIN_ALL 16

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

/**
 * @brief 获取对应 LEDX_ON_L 输出通道的寄存器地址 (datasheet P20)
 * @param pin 引脚pin
 */
static int baseReg(int pin)
{
    // 计算获得对应引脚寄存器地址 (见datasheet P9)
    return (pin >= PIN_ALL ? LEDALL_ON_L : LED0_ON_L + 4 * pin);
}

/**
  * @brief  设置所有输出为0
  * @param  fd 文件描述符
  */
static void pca9685PWMReset(int fd)
{
    wiringPiI2CWriteReg16(fd, LEDALL_ON_L, 0x0);        // ALL_LED full ON  失能
    wiringPiI2CWriteReg16(fd, LEDALL_ON_L + 2, 0x1000); // ALL_LED full OFF 使能
}

/**
  * @brief  向pca9685对应 pin 写入PWM值
  * @param  fd 文件描述符
  * @param  pin 对应引脚pin
  */
static void pca9685PWMWrite(int fd, int pin, int on, int off)
{
    int reg = baseReg(pin);

    // 可写入位 12bit，最大值为 4095    on + off = 4095
    wiringPiI2CWriteReg16(fd, reg, on & 0x0FFF);
    wiringPiI2CWriteReg16(fd, reg + 2, off & 0x0FFF);
}

/**
  * @brief  对应 pin 输出0(相当于当做普通io口用)
  * @param  fd  文件描述符
  * @param  pin  引脚pin
  * @param  tf  tf设置
  */
static void pca9685FullOff(int fd, int pin, int tf)
{
    int reg = baseReg(pin) + 3; // LEDX_OFF_H 寄存器
    int state = wiringPiI2CReadReg8(fd, reg);

    // 根据 tf 设置 第4bit 为 0 or 1
    state = tf ? (state | 0x10) : (state & 0xEF);

    wiringPiI2CWriteReg8(fd, reg, state);
}

/**
  * @brief  对应 pin 输出1(相当于当做普通io口用)
  * @param  fd  文件描述符
  * @param  pin  引脚pin
  * @param  tf  tf设置
  */
static void pca9685FullOn(int fd, int pin, int tf)
{
    int reg = baseReg(pin) + 1; // LEDX_ON_H 寄存器
    int state = wiringPiI2CReadReg8(fd, reg);

    // 根据 tf 设置 第4bit 为 0 or 1
    state = tf ? (state | 0x10) : (state & 0xEF);

    wiringPiI2CWriteReg8(fd, reg, state);

    //  full-off 优先级高于 full-on (datasheet P23)
    if (tf)
        pca9685FullOff(fd, pin, 0);
}

/**
  * @brief  pwm 写入
  * @param  node  节点
  * @param  pin  引脚pin
  * @param  value 写入值
  */
static void myPwmWrite(struct wiringPiNodeStruct *node, int pin, int value)
{
    int fd = node->fd;
    int ipin = pin - node->pinBase;

    if (value >= 4096) pca9685FullOn(fd, ipin, 1);
    else if (value > 0) pca9685PWMWrite(fd, ipin, 0, value);
    else pca9685FullOff(fd, ipin, 1);
}

/**
  * @brief  数字量写入 (相当于当做普通io口)
  * @param  node  节点
  * @param  pin  引脚pin
  * @param  value 写入值
  */
static void myOnOffWrite(struct wiringPiNodeStruct *node, int pin, int value)
{
    int fd = node->fd;
    int ipin = pin - node->pinBase;

    if (value) pca9685FullOn(fd, ipin, 1);
    else pca9685FullOff(fd, ipin, 1);
}

/**
  * @brief  pca9685 设置PWM频率
  * @param  fd 文件描述符
  * @param  freg 频率
  */
void pca9685PWMSetFreq(int fd, float freq, float pwm_calibration)
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
    int prescale = (int)((PCA9685_OSC_CLK / (4096 * freq)+0) /(pwm_calibration+1.0f)); // 1.034校准

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

/**
 * @brief  初始化并设置 pca9685
 * @param  pinBase  pinBase > 64（wiringPi 中，pin编号小于64认为是板载GPIO，如果编号大于64则认为是外扩GPIO
 * 	因此 pinBase > 64）
 * @param  freq   频率范围在 [40, 1000]
 */
int pca9685Setup(const int pinBase, float freq)
{   
    int fd;
    int settings, autoInc;
    struct wiringPiNodeStruct *node = NULL; // 指针初始化为NULL，以免产生段错误

    // PCA9685_OE_PIN，即 GPIOG11 低电平使能
    pinMode(PCA9685_OE_PIN, OUTPUT);
    digitalWrite(PCA9685_OE_PIN, LOW);

    // 小于0代表无法找到该i2c接口，输入命令 sudo npi-config 使能该i2c接口
    fd = wiringPiI2CSetupInterface(PCA9685_I2C_DEV, PCA9685_I2C_ADDR);
    if (fd < 0)
        return -1;

    /* 检测是否存在 pca9685 器件
     * 小于0代表读取失败，代表不存在该 PCA9685 器件，或者器件地址错误
     */
    if ((settings = wiringPiI2CReadReg8(fd, PCA9685_MODE1)) < 0)
        return -2;

    autoInc = (settings | 0x20) & 0x7F;
    wiringPiI2CWriteReg8(fd, PCA9685_MODE1, autoInc);

    if (freq < 40 || freq > 1000)
    {
        log_e("PCA9685 freq range in [40, 1000]");
        freq = 50;
    }
        
    // 设置 PWM 频率，启动输出
    pca9685PWMSetFreq(fd, freq, 0.0f);

    // 创建节点 16 pins [0..15] + [16] for all
    node = wiringPiNewNode(pinBase, PIN_ALL + 1);
    if (!node)
        return -4;

    // 注册方法
    node->fd = fd;
    node->pwmWrite = myPwmWrite;
    node->digitalWrite = myOnOffWrite;

    // 重置所有输出
    pca9685PWMReset(fd);

    return fd;
}

#ifdef __cplusplus
}
#endif
