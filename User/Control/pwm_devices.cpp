#include "pwm_devices.h"

PWMDevice::PWMDevice(const std::string &name, int channel) noexcept
    : m_channel(channel)
{
    m_params = Global<Config>::Get()
                   ->getValue(name,
                              PWMDeviceParams{.speed = 500,
                                              .cur = 1500,
                                              .mid = 1300,
                                              .pMax = 1500,
                                              .nMax = 900},
                              [this](const Json &old_value, const Json &new_value)
                              {
                                  this->m_params = new_value;
                                  this->resetPosition();
                              });
}

void PWMDevice::resetPosition() noexcept
{
    m_state = State::Reset;
}

void PWMDevice::positive() noexcept
{
    m_state = State::GoPositive;
}

void PWMDevice::negitive() noexcept
{
    m_state = State::GoNegitive;
}

PWMDevice::Type PWMDevice::getType()
{
    return m_type;
}

void PWMDevice::doResetPosition() noexcept
{
    auto del_us = m_params.mid - m_params.cur;
    auto del_fre = abs(del_us) > m_params.speed ? m_params.speed : del_us;
    if (del_us > 0)
        m_params.cur += abs(del_fre);
    else
        m_params.cur -= abs(del_fre);
}

void PWMDevice::doPositive() noexcept
{
    auto del_us = m_params.pMax - m_params.cur;
    auto del_fre = del_us > m_params.speed ? m_params.speed : del_us;
    m_params.cur += del_fre;
}

void PWMDevice::doNegitive() noexcept
{
    auto del_us = m_params.cur - m_params.nMax;
    auto del_fre = del_us > m_params.speed ? m_params.speed : del_us;
    m_params.cur -= del_fre;
}

void PWMDevice::run() noexcept
{
    using enum State;
    switch (m_state)
    {
    case GoNegitive:
        this->doNegitive();
        if (m_params.cur == m_params.pMax)
            m_state = Stop;
        break;
    case GoPositive:
        this->doPositive();
        if (m_params.cur == m_params.pMax)
            m_state = Stop;
        break;
    case Reset:
        this->doResetPosition();
        if (m_params.cur == m_params.mid)
            m_state = Stop;
        break;
    default:
        break;
    }
}
