#ifndef __USER_CONTROL_PROPELLER_H__
#define __USER_CONTROL_PROPELLER_H__

#include <memory>

#include "User/macro.h"

class EventPCA9685;
class Control;

struct PropellerAttribute
{
    int deadZoneUpper = 0;      // 死区上限
    int deadZoneLower = 0;      // 死区下限
    double powerPositive = 0.8; // 正向动力百分比
    double powerNegative = 0.8; // 反向动力百分比
    int reversed = false;       // 反转
    int enabled = true;         // 启用/禁用推进器
};

struct PropellerGroup
{
    PropellerAttribute frontRight;
    PropellerAttribute middleRight;
    PropellerAttribute backRight;
    PropellerAttribute frontLeft;
    PropellerAttribute middleLeft;
    PropellerAttribute backLeft;
};

struct ControlLoopParameters
{
    PIDParameters depthLock;
    PIDParameters directionLock;
};

struct PropellerParameters
{
    double pwmFreqCalibration;
    PropellerGroup propellerGroup;
    ControlLoopParameters controlLoopParameters;
};

class PropellerControlBase
{
public:
    friend EventPCA9685;
    friend Control;
    using ptr = std::unique_ptr<PropellerControlBase>;

    PropellerControlBase(int PWMmed = 1500, int PWMPositiveMax = 2000, int PWMNegitiveMax = 1000);
    virtual ~PropellerControlBase() = default;

    virtual void move(float rocker_x, float rocker_y, float rocker_z, float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept;
    virtual void move_absolute(float rocker_x, float rocker_y, float rocker_z, float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept;
    virtual void move_relative(float rocker_x, float rocker_y, float rocker_z, float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept;
    void to_depth(float depth) noexcept;
    void set_direction_locked(float val) noexcept;
    void set_depth_locked(float val) noexcept;

protected:
    struct PropellerPower // 传递给Event 往PCA9685写数据  请勿移动此结构体内变量的顺序！！！
    {                     // 统一正数向前
        int16_t frontRight;
        int16_t middleRight;
        int16_t backRight;
        int16_t frontLeft;
        int16_t middleLeft;
        int16_t backLeft;
    };

    struct RockerData
    {
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;
        int16_t yaw = 0;
        int16_t roll = 0;
        int16_t pitch = 0;
    };

    struct ExpectAttitude // 对应的标志位为1时 有效
    {
        float depth; // <--让传感器MS5837到达的值
        float yaw;   // <--让countAttitude到达的值
        float roll;  // <--让countAttitude到达的值
        float pitch; // <--让countAttitude到达的值
    };

    struct Attitude
    {
        float yaw = 0;
        float roll = 0;
        float pitch = 0;
    };

    enum Flag
    {
        DepthLockFlag = BITS(0),
        YawLockFlag = BITS(1),
        RollLockFlag = BITS(2),
        PitchLockFlag = BITS(3),

        ExpectYawFlag = BITS(4),
        ExpectDepthFlag = BITS(5),
        ExpectRollFlag = BITS(6),
        ExpectPitchFlag = BITS(7)
    };

    uint32_t m_flags = 0;
    RockerData m_rockerBuffer;
    Attitude m_lastAttitude;
    Attitude m_delAttitude;
    PropellerGroup m_params;
    PropellerPower m_powerOutput{};
    ExpectAttitude m_expectAttitude{};
    int m_PWMmed;
    int m_PWMPositiveMax;
    int m_PWMNegitiveMax;

protected:
    void parse_rocker(float rocker_x, float rocker_y, float rocker_z, float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept;
    void final_handle(); // 符号处理 && 功率输出系数 &&  死区补偿 && 转换 && 限幅
    void refreshData() noexcept;
    virtual void do_xPower(int16_t x_power) noexcept;
    virtual void do_yPower(int16_t y_power) noexcept;
    virtual void do_zPower(int16_t z_power) noexcept;
    virtual void do_yawPower(int16_t yaw_power) noexcept;
    virtual void do_rollPower(int16_t roll_power) noexcept;
    virtual void do_pitchPower(int16_t pitch_power) noexcept;
    virtual void run() noexcept = 0;

private:
    const int16_t *get6RawData() noexcept; // 注意 长度为6
};

class PropellerControlV1 : public PropellerControlBase
{
public:
    PropellerControlV1(int m_PWMmed = 1500, int m_PWMPositiveMax = 2000, int m_PWMNegitiveMax = 1000);

private:
    void run() noexcept override;
};

class PropellerControlV2 : public PropellerControlBase
{
private:
    void run() noexcept override;
};

#endif
