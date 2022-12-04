#include "control_propeller.h"

#include <cstring>
#include <jy901.h>
#include <ms5837.h>
#include <pca9685.h>

#include "User/macro.h"
#include "User/config.h"
#include "User/utils.h"
#include "User/log.h"

PropellerControlBase::PropellerControlBase(int PWMmed, int PWMPositiveMax, int PWMNegitiveMax)
    : m_PWMmed(PWMmed),
      m_PWMPositiveMax(PWMPositiveMax),
      m_PWMNegitiveMax(PWMNegitiveMax)
{
    m_params = Global<Config>::Get()->getValue("propeller",
                                               PropellerParameters(),
                                               [this](const Json &old_value, const Json &new_value)
                                               {
                                                   // Global<PCA9685>::Get()->setPwmFreq();
                                                   this->m_params = new_value.at("propeller_parameters");
                                                   LOG_INFO("成功加载 propeller");
                                               })
                   .propellerGroup;
}

const int16_t *PropellerControlBase::get6RawData() noexcept
{
    return (int16_t *)&m_powerOutput;
}

void PropellerControlBase::to_depth(float depth) noexcept
{
    m_flags |= ExpectDepthFlag;
    m_expectAttitude.depth = depth;
}

void PropellerControlBase::set_direction_locked(float val) noexcept
{
    if (val > 0)
        m_flags = (m_flags | YawLockFlag);
    else
        m_flags = (m_flags & (~YawLockFlag));
}

void PropellerControlBase::set_depth_locked(float val) noexcept
{
    if (val > 0)
        m_flags = Flag(m_flags | DepthLockFlag);
    else
        m_flags = Flag(m_flags & (~DepthLockFlag));
}

void PropellerControlBase::move_absolute(float rocker_x, float rocker_y, float rocker_z, float rot) noexcept
{
    m_flags = m_flags | ExpectYawFlag;

    if (rot < 0.0 || rot >= 360.0)
        return;

    float cur_rot = Global<JY901>::Get()->getYaw();
    int del_yaw = rot - cur_rot;

    if (abs(del_yaw) > 180)
        del_yaw -= 360;

    m_expectAttitude.yaw = m_countAttitude.yaw + del_yaw;

    parse_rocker(rocker_x, rocker_y, rocker_z, 0.0f);
}

void PropellerControlBase::move_relative(float rocker_x, float rocker_y, float rocker_z, float rot) noexcept
{
    m_flags = m_flags | ExpectYawFlag;
    m_expectAttitude.yaw = m_countAttitude.yaw + rot;

    parse_rocker(rocker_x, rocker_y, rocker_z, 0.0f);
}

void PropellerControlBase::move(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept
{
    int tmp_flag = 0; // 将要消除哪些标志位
    if (rocker_rot > 1.0 / 400)
        tmp_flag |= ExpectYawFlag;
    if (rocker_z > 1.0 / 400)
        tmp_flag |= ExpectDepthFlag;
    m_flags = m_flags & (~tmp_flag);

    parse_rocker(rocker_x, rocker_y, rocker_z, rocker_rot);
}

void PropellerControlBase::parse_rocker(float rocker_x, float rocker_y,
                                        float rocker_z, float rocker_rot) noexcept
{
    m_rockerBuffer.x = (int16_t)(rocker_x * 400);
    m_rockerBuffer.y = (int16_t)(rocker_y * 400);
    m_rockerBuffer.z = (int16_t)(rocker_y * 400);
    m_rockerBuffer.rot = (int16_t)(rocker_rot * 400);
}

void PropellerControlBase::refreshData() noexcept
{
    auto current_pitch = Global<JY901>::Get()->getPitch();
    auto current_roll = Global<JY901>::Get()->getRoll();
    auto current_yaw = Global<JY901>::Get()->getYaw();
    m_countAttitude.pitch = current_pitch - m_lastAttitude.pitch;
    m_countAttitude.roll = current_roll - m_lastAttitude.roll;
    m_countAttitude.yaw = current_yaw - m_lastAttitude.yaw;
    m_lastAttitude.pitch = current_pitch;
    m_lastAttitude.roll = current_roll;
    m_lastAttitude.yaw = current_yaw;

    memset(&m_powerOutput, 0, sizeof(m_powerOutput));
}

void PropellerControlBase::do_xPower(int16_t x_power) noexcept
{
    /******左右********
     *
     *   动力给正 右平移
     *
     *      / (\反向)
     *     |    |
     * (反向\)  /
     *
     */
    m_powerOutput.frontLeft += x_power;
    m_powerOutput.frontRight -= x_power;
    m_powerOutput.backLeft -= x_power;
    m_powerOutput.backRight += x_power;
}

void PropellerControlBase::do_yPower(int16_t y_power) noexcept
{
    /**前后**/
    m_powerOutput.frontLeft += y_power;
    m_powerOutput.frontRight += y_power;
    m_powerOutput.backLeft += y_power;
    m_powerOutput.backRight += y_power;
}

void PropellerControlBase::do_zPower(int16_t z_power) noexcept
{
    m_powerOutput.middleLeft += z_power;
    m_powerOutput.middleRight += z_power;
}

void PropellerControlBase::do_yawPower(int16_t yaw_power) noexcept
{
    m_powerOutput.frontLeft += 1.414 / 2 * yaw_power;
    m_powerOutput.frontRight += 1.414 / 2 * yaw_power;
    m_powerOutput.backLeft += 1.414 / 2 * yaw_power;
    m_powerOutput.backRight += 1.414 / 2 * yaw_power;
}

/*可以想象出一边做这个动作一边开垂推会做绕一侧运动（质心在全机器中心的理想情况）*/
void PropellerControlBase::do_rollPower(int16_t roll_power) noexcept
{
    if (roll_power > 0)
    {
        m_powerOutput.middleLeft -= roll_power;
        m_powerOutput.middleRight += roll_power;
    }
    else
    {
        m_powerOutput.middleLeft += roll_power;
        m_powerOutput.middleRight -= roll_power;
    }
}

void PropellerControlBase::do_pitchPower(int16_t pitch_power) noexcept
{
    // 做不到 留着  除非四发垂推或者一前一后
}

void PropellerControlBase::final_handle()
{
    // 顺序不能乱 1.乘上输出功率系数  2.换符号(符号正代表往前 网上)<-|->（符号正代表顺时针转 正转则占空超过1500加死区补偿）3.失能掉禁用的推进器  4.死区补偿 5.加上中间量  6.限幅
    // No.1 ******************一一一一一一一一一一一一一一****************************
#define XX(position) \
    m_powerOutput.position *= m_powerOutput.position > 0 ? m_params.position.powerPositive : m_params.position.powerNegative;

    XX(backLeft);
    XX(backRight);
    XX(middleLeft);
    XX(middleRight);
    XX(frontLeft);
    XX(frontRight);
#undef XX

    // No.2 ******************二二二二二二二二二二二二二二****************************
    m_powerOutput.frontLeft *= -1;
    m_powerOutput.middleLeft *= -1;
    m_powerOutput.backLeft *= -1;

#define XX(position)                \
    if (m_params.position.reversed) \
        m_powerOutput.position *= -1;

    XX(backLeft);
    XX(backRight);
    XX(middleLeft);
    XX(middleRight);
    XX(frontLeft);
    XX(frontRight);
#undef XX
    // No.3 *******************三三三三三三三三三三三三三三**************************
#define XX(position)                \
    if (!m_params.position.enabled) \
        m_powerOutput.position = 0;

    XX(backLeft);
    XX(backRight);
    XX(middleLeft);
    XX(middleRight);
    XX(frontLeft);
    XX(frontRight);
#undef XX
    // No.4 *******************四四四四四四四四四四四四四四**************************
#define XX(position)                                               \
    if (m_powerOutput.position > 0)                                \
        m_powerOutput.position += m_params.position.deadZoneUpper; \
    else if (m_powerOutput.position < 0)                           \
        m_powerOutput.position -= m_params.position.deadZoneLower;

    XX(backLeft);
    XX(backRight);
    XX(middleLeft);
    XX(middleRight);
    XX(frontLeft);
    XX(frontRight);
#undef XX
    // No.5 *******************五五五五五五五五五五五五五五**************************
#define XX(position) \
    m_powerOutput.position += m_PWMmed

    XX(backLeft);
    XX(backRight);
    XX(middleLeft);
    XX(middleRight);
    XX(frontLeft);
    XX(frontRight);
#undef XX
    // No.6 *******************六六六六六六六六六六六六六六**************************
#define XX(position) \
    m_powerOutput.position = constrain(m_powerOutput.position, m_PWMNegitiveMax, m_PWMPositiveMax)

    XX(backLeft);
    XX(backRight);
    XX(middleLeft);
    XX(middleRight);
    XX(frontLeft);
    XX(frontRight);
#undef XX
}

PropellerControlV1::PropellerControlV1(int PWMmed, int PWMPositiveMax, int PWMNegitiveMax)
    : PropellerControlBase(PWMmed, PWMPositiveMax, PWMNegitiveMax)
{
}

void PropellerControlV1::run() noexcept
{
    /**********************更新**************************/
    this->refreshData();

    /********************锁定->期望**********************/
    if (m_flags & DepthLockFlag && !(m_flags & ExpectDepthFlag))
    {
        m_flags |= ExpectDepthFlag;
        m_expectAttitude.depth = Global<MS5837>::Get()->getDepth();
    }
    if (m_flags & YawLockFlag && !(m_flags & ExpectYawFlag))
    {
        m_flags |= ExpectYawFlag;
        m_expectAttitude.yaw = m_countAttitude.yaw;
    }
    // if (m_flags & RollLockFlag && !(m_flags & ExpectRollFlag))
    // {
    //     m_flags |= ExpectRollFlag;
    //     m_expectAttitude.roll = m_countAttitude.roll;
    // }
    // if (m_flags & PitchLockFlag && !(m_flags & ExpectPitchFlag))
    // {
    //     m_flags |= ExpectPitchFlag;
    //     m_expectAttitude.pitch = m_countAttitude.pitch;
    // }

    /************************期望****************************/
    if (m_flags & ExpectDepthFlag && Global<MS5837>::Get()->isValid())
    {
        auto del_depth = m_expectAttitude.depth - Global<MS5837>::Get()->getDepth();
        do_zPower(del_depth);
    }

    do
    {
        if (!Global<JY901>::Get()->isValid())
            break;
        if (m_flags & ExpectYawFlag)
        {
            auto del_yaw = m_expectAttitude.yaw - m_countAttitude.yaw;
            do_yawPower(del_yaw);
        }
        // if (m_flags & ExpectRollFlag)
        // {
        //     auto del_roll = m_expectAttitude.roll - m_countAttitude.roll;
        //     do_rollPower(del_roll);
        // }
        // if (m_flags & ExpectPitchFlag)
        // {
        //     auto del_pitch = m_expectAttitude.pitch - m_countAttitude.pitch;
        //     do_pitchPower(del_pitch);
        // }
    } while (false);

    /*************************手柄操控****************************/ // x左右 y前后 z沉浮
    do_xPower(m_rockerBuffer.x);
    do_yPower(m_rockerBuffer.y);
    do_zPower(m_rockerBuffer.z);
    do_yawPower(m_rockerBuffer.rot);
    /**************************善后*******************************/
    final_handle();
}

// 改日再TODO
// void PropellerControlV2::run() noexcept
// {
//     auto current_pitch = Global<JY901>::Get()->getPitch();
//     auto current_roll = Global<JY901>::Get()->getRoll();
//     auto current_yaw = Global<JY901>::Get()->getYaw();
//     m_countAttitude.pitch = current_pitch - m_lastAttitude.pitch;
//     m_countAttitude.roll = current_roll - m_lastAttitude.roll;
//     m_countAttitude.yaw = current_yaw - m_lastAttitude.yaw;
//     m_lastAttitude.pitch = current_pitch;
//     m_lastAttitude.roll = current_roll;
//     m_lastAttitude.yaw = current_yaw;

//     if (m_flags & ExpectDepthFlag)
//     {
//         auto del_depth = m_expectAttitude.depth - Global<MS5837>::Get()->getDepth();
//     }

//     if (m_flags & ExpectYawFlag)
//     {
//     }

//     if (m_flags & ExpectRollFlag)
//     {
//     }

//     if (m_flags & ExpectPitchFlag)
//     {
//     }

//     m_powerOutput.frontLeft *= -1;
//     m_powerOutput.middleLeft *= -1;
//     m_powerOutput.backLeft *= -1;
// }
