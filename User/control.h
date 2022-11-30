#ifndef __USER_CONTROL_H__
#define __USER_CONTROL_H__

#include <array>
#include <tuple>
#include <cstdint>

struct PropellerAttribute
{
    int deadZoneUpper;     // 死区上限
    int deadZoneLower;     // 死区下限
    double powerPositive;  // 正向动力百分比
    double powerNegative;  // 反向动力百分比
    int reversed;          // 反转
    int enabled;           // 启用/禁用推进器
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

struct PropellerParameters
{
    double pwmFreqCalibration;
    PropellerGroup propellerGroup;
};

class ControlBase
{
public:
    ControlBase() = default;
    virtual ~ControlBase() = default;

    virtual void move(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept = 0;
    virtual void move_absolute(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept = 0;
    virtual void move_relative(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept = 0;
    void catcher(float val) noexcept;
    void depth(float depth) noexcept;
    void set_direction_locked() noexcept;
    void set_depth_locked() noexcept;

    const int16_t *get6RawData() noexcept; // 注意 长度为6
protected:
    struct PropellerPower
    {
        int16_t frontRight;
        int16_t middleRight;
        int16_t backRight;
        int16_t frontLeft;
        int16_t middleLeft;
        int16_t backLeft;
    };

    std::array<int16_t, 4> parase_rocker(float rocker_x, float rocker_y, float rocker_z, float rocker_rot);
    PropellerPower m_propellerPower;
};

class ControlV2 : public ControlBase
{
public:
    void move(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept override;
    void move_absolute(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept override;
    void move_relative(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept override;
private:
};

#endif