#include "control_propeller.h"

#include <cstring>
#include <jy901.h>
#include <ms5837.h>
#include <pca9685.h>
#include <cmath>

#include "User/macro.h"
#include "User/config.h"
#include "User/utils.h"
#include "User/log.h"

PropellerControlBase::PropellerControlBase(int PWMmed, int PWMPositiveMax, int PWMNegitiveMax)
        : m_PWMmed(PWMmed),
          m_PWMPositiveMax(PWMPositiveMax),
          m_PWMNegitiveMax(PWMNegitiveMax) {
    m_params = Global<Config>::Get()->getValue("propeller",
                                               PropellerParameters(),
                                               [this](const Json &old_value, const Json &new_value) {
                                                   // Global<PCA9685>::Get()->setPwmFreq();
                                                   this->m_params = new_value.at("propeller_parameters");
                                                   LOG_INFO("成功加载 propeller");
                                               })
            .propellerGroup;

    int pinBase = Global<PCA9685>::Get()->getPinBase();
    for (size_t i = 0; i < 6; ++i) {
        Global<PCA9685>::Get()->pwmWrite(pinBase + i, PCA9685::CalcTicks(1550));
    }

    sleep(0.5); // 2s

    for (size_t i = 0; i < 6; i++) {
        Global<PCA9685>::Get()->pwmWrite(pinBase + i, PCA9685::CalcTicks(1500));
    }

    sleep(1); // 2s
}

const int16_t *PropellerControlBase::get6RawData() noexcept {
    return (int16_t *) &m_powerOutput;
}

void PropellerControlBase::to_depth(float depth) noexcept {
    m_flags |= ExpectDepthFlag;
    m_expectAttitude.depth = depth;
}

void PropellerControlBase::set_direction_locked(float val) noexcept {
    if (val > 0)
        m_flags = (m_flags | YawLockFlag);
    else
        m_flags = (m_flags & (~YawLockFlag));
}

void PropellerControlBase::set_depth_locked(float val) noexcept {
    if (val > 0)
        m_flags = Flag(m_flags | DepthLockFlag);
    else
        m_flags = Flag(m_flags & (~DepthLockFlag));
}

void PropellerControlBase::move_absolute(float rocker_x, float rocker_y, float rocker_z, float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept {
    m_flags = m_flags | ExpectYawFlag;

    if (rocker_yaw < 0.0 || rocker_yaw >= 360.0)
        return;

    float cur_rot = Global<JY901>::Get()->getYaw();
    float del_yaw = rocker_yaw - cur_rot;

    if (fabs(del_yaw) > 180.0f)
        del_yaw -= 360;

    m_expectAttitude.yaw = m_delAttitude.yaw + del_yaw;

    parse_rocker(rocker_x, rocker_y, rocker_z, rocker_yaw, rocker_roll,rocker_pitch);
}

void
PropellerControlBase::move_relative(float rocker_x, float rocker_y, float rocker_z,
                                    float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept {
    m_flags = m_flags | ExpectYawFlag;
    m_expectAttitude.yaw = m_delAttitude.yaw + rocker_yaw;

    parse_rocker(rocker_x, rocker_y, rocker_z, rocker_yaw, rocker_roll, rocker_pitch);
}

void PropellerControlBase::move(float rocker_x, float rocker_y, float rocker_z,
                                float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept {
    int tmp_flag = 0; // 将要消除哪些标志位
    if (rocker_yaw > 1.0 / 400)
        tmp_flag |= ExpectYawFlag;
    if (rocker_z > 1.0 / 400)
        tmp_flag |= ExpectDepthFlag;
    m_flags = m_flags & (~tmp_flag);

    parse_rocker(rocker_x, rocker_y, rocker_z, rocker_yaw, rocker_roll, rocker_pitch);
}

void PropellerControlBase::parse_rocker(float rocker_x, float rocker_y, float rocker_z,
                                        float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept {
    m_rockerBuffer.x = (int16_t) (rocker_x * 400);
    m_rockerBuffer.y = (int16_t) (rocker_y * 400);
    m_rockerBuffer.z = (int16_t) (rocker_y * 400);
    m_rockerBuffer.yaw = (int16_t) (rocker_yaw * 400);
}

void PropellerControlBase::refreshData() noexcept {
    auto current_pitch = Global<JY901>::Get()->getPitch();
    auto current_roll = Global<JY901>::Get()->getRoll();
    auto current_yaw = Global<JY901>::Get()->getYaw();
    m_delAttitude.pitch = current_pitch - m_lastAttitude.pitch;
    m_delAttitude.roll = current_roll - m_lastAttitude.roll;
    m_delAttitude.yaw = current_yaw - m_lastAttitude.yaw;
    m_lastAttitude.pitch = current_pitch;
    m_lastAttitude.roll = current_roll;
    m_lastAttitude.yaw = current_yaw;

    memset(&m_powerOutput, 0, sizeof(m_powerOutput));
}

void PropellerControlBase::do_xPower(int16_t x_power) noexcept {
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

void PropellerControlBase::do_yPower(int16_t y_power) noexcept {
    /**前后**/
    m_powerOutput.frontLeft += y_power;
    m_powerOutput.frontRight += y_power;
    m_powerOutput.backLeft += y_power;
    m_powerOutput.backRight += y_power;
}

void PropellerControlBase::do_zPower(int16_t z_power) noexcept {
    m_powerOutput.middleLeft += z_power;
    m_powerOutput.middleRight += z_power;
}

void PropellerControlBase::do_yawPower(int16_t yaw_power) noexcept {
    m_powerOutput.frontLeft += 1.414 / 2 * yaw_power;
    m_powerOutput.frontRight += 1.414 / 2 * yaw_power;
    m_powerOutput.backLeft += 1.414 / 2 * yaw_power;
    m_powerOutput.backRight += 1.414 / 2 * yaw_power;
}

/*可以想象出一边做这个动作一边开垂推会做绕一侧运动（质心在全机器中心的理想情况）*/
void PropellerControlBase::do_rollPower(int16_t roll_power) noexcept {
    if (roll_power > 0) {
        m_powerOutput.middleLeft -= roll_power;
        m_powerOutput.middleRight += roll_power;
    } else {
        m_powerOutput.middleLeft += roll_power;
        m_powerOutput.middleRight -= roll_power;
    }
}

void PropellerControlBase::do_pitchPower(int16_t pitch_power) noexcept {
    // 做不到 留着  除非四发垂推或者一前一后
}

#define DO_FOR_EVERY_PROPELLER(action) \
    action(backLeft); \
    action(backRight);\
    action(middleLeft);\
    action(middleRight);\
    action(frontLeft);\
    action(frontRight);

#define DISABLE_UNUSED_PROPELLER(position) \
    if (!m_params.position.enabled) \
    m_powerOutput.position = 0;

#define MULTIPLY_POWER_COEFFICIENT(position) \
    m_powerOutput.position *= m_powerOutput.position > 0 ? m_params.position.powerPositive : m_params.position.powerNegative;

#define CHANGE_SIGN(position) \
    if (m_params.position.reversed) \
    m_powerOutput.position *= -1;

#define ADD_DEAD_ZONE(position) \
    if (m_powerOutput.position > 0)                                \
        m_powerOutput.position += m_params.position.deadZoneUpper; \
    else if (m_powerOutput.position < 0)                           \
        m_powerOutput.position -= m_params.position.deadZoneLower;

#define ADD_PWM_MED(position) \
    m_powerOutput.position += m_PWMmed

#define CONSTRAIN(position) \
    m_powerOutput.position = constrain(m_powerOutput.position, m_PWMNegitiveMax, m_PWMPositiveMax)

void PropellerControlBase::final_handle() {
    m_powerOutput.frontLeft *= -1;
    m_powerOutput.middleLeft *= -1;
    m_powerOutput.backLeft *= -1;

    DO_FOR_EVERY_PROPELLER(DISABLE_UNUSED_PROPELLER)
    DO_FOR_EVERY_PROPELLER(MULTIPLY_POWER_COEFFICIENT)
    DO_FOR_EVERY_PROPELLER(CHANGE_SIGN)
    DO_FOR_EVERY_PROPELLER(ADD_DEAD_ZONE)
    DO_FOR_EVERY_PROPELLER(ADD_PWM_MED)
    DO_FOR_EVERY_PROPELLER(CONSTRAIN)
}

PropellerControlV1::PropellerControlV1(int PWMmed, int PWMPositiveMax, int PWMNegitiveMax)
        : PropellerControlBase(PWMmed, PWMPositiveMax, PWMNegitiveMax) {
}

void PropellerControlV1::run() noexcept {
    /**********************更新**************************/
    this->refreshData();

    /********************锁定->期望**********************/
    if (m_flags & DepthLockFlag && !(m_flags & ExpectDepthFlag)) {
        m_flags |= ExpectDepthFlag;
        m_expectAttitude.depth = Global<MS5837>::Get()->getDepth();
    }
    if (m_flags & YawLockFlag && !(m_flags & ExpectYawFlag)) {
        m_flags |= ExpectYawFlag;
        m_expectAttitude.yaw = m_delAttitude.yaw;
    }
    // if (m_flags & RollLockFlag && !(m_flags & ExpectRollFlag))
    // {
    //     m_flags |= ExpectRollFlag;
    //     m_expectAttitude.roll = m_delAttitude.roll;
    // }
    // if (m_flags & PitchLockFlag && !(m_flags & ExpectPitchFlag))
    // {
    //     m_flags |= ExpectPitchFlag;
    //     m_expectAttitude.pitch = m_delAttitude.pitch;
    // }

    /************************期望****************************/
    if (m_flags & ExpectDepthFlag && Global<MS5837>::Get()->isValid()) {
        auto del_depth = m_expectAttitude.depth - Global<MS5837>::Get()->getDepth();
        // 添加 PID
        do_zPower(del_depth);
    }

    do {
        if (!Global<JY901>::Get()->isValid())
            break;
        if (m_flags & ExpectYawFlag) // 添加 PID
            do_yawPower(m_delAttitude.yaw);
//        if (m_flags & ExpectRollFlag) // 添加 PID
//            do_rollPower(m_delAttitude.roll);
//        if (m_flags & ExpectPitchFlag) // 添加 PID
//            do_pitchPower(m_delAttitude.pitch);
    } while (false);

    /*************************手柄操控****************************/ // x左右 y前后 z沉浮
    do_xPower(m_rockerBuffer.x);
    do_yPower(m_rockerBuffer.y);
    do_zPower(m_rockerBuffer.z);
    do_yawPower(m_rockerBuffer.yaw);
    /**************************善后*******************************/
    final_handle();
}

// 改日再TODO
// void PropellerControlV2::run() noexcept
// {
//     auto current_pitch = Global<JY901>::Get()->getPitch();
//     auto current_roll = Global<JY901>::Get()->getRoll();
//     auto current_yaw = Global<JY901>::Get()->getYaw();
//     m_delAttitude.pitch = current_pitch - m_lastAttitude.pitch;
//     m_delAttitude.roll = current_roll - m_lastAttitude.roll;
//     m_delAttitude.yaw = current_yaw - m_lastAttitude.yaw;
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
