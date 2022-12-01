#ifndef __USER__CONTROL_PWM_DEVICES_H__
#define __USER__CONTROL_PWM_DEVICES_H__

#include <memory>

#include "User/Event/control_pca9685.h"

class Control;

struct PWMDeviceParams
{
    int speed;
    int cur;
    int mid;
    int pMax;
    int nMax;
};

class PWMDevice
{
public:
    friend EventPCA9685;
    friend Control;
    using ptr = std::unique_ptr<PWMDevice>;

    enum class Type
    {
        Catch,
        YunTai,
        Light
    };

    PWMDevice(const std::string &name, int channel) noexcept;

    void resetPosition() noexcept;
    void positive() noexcept;
    void negitive() noexcept;

    Type getType();

private:
    void doResetPosition() noexcept;
    void doPositive() noexcept;
    void doNegitive() noexcept;
    void run() noexcept;
private:
    enum class State
    {
        GoPositive,
        GoNegitive,
        Reset,
        Stop
    };

    int m_channel;
    PWMDeviceParams m_params;
    State m_state;
    Type m_type;
};

#endif