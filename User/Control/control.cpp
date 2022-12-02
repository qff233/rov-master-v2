#include "control.h"

#include <easylogging++.h>
#include <wiringPi.h>
#include <pca9685.h>

#include "control_propeller.h"
#include "pwm_devices.h"
#include "User/utils.h"

Control::Control(PropellerControlBase::ptr &&propellerControl)
{
    m_propeller = std::move(propellerControl);
}

Control::~Control()
{
    this->stop();
}

const int16_t *Control::get6RawData() noexcept
{
    return m_propeller->get6RawData();
}

void Control::move(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept
{
    m_propeller->move(rocker_x, rocker_y, rocker_z, rocker_rot);
}

void Control::move_absolute(float rocker_x, float rocker_y, float rocker_z, float rot) noexcept
{
    m_propeller->move_absolute(rocker_x, rocker_y, rocker_z, rot);
}

void Control::move_relative(float rocker_x, float rocker_y, float rocker_z, float rot) noexcept
{
    m_propeller->move_relative(rocker_x, rocker_y, rocker_z, rot);
}

void Control::catcher(float val) noexcept
{
    if (val == 0)
        return;

    for (auto &i : m_pwmDevices)
    {
        if (i->getType() == PWMDevice::Type::Catch)
            if (val > 0)
                i->positive();
            else if (val < 0)
                i->negitive();
    }
}

void Control::to_depth(float depth) noexcept
{
    m_propeller->to_depth(depth);
}

void Control::set_direction_locked(float val) noexcept
{
    m_propeller->set_direction_locked(val);
}

void Control::set_depth_locked(float val) noexcept
{
    m_propeller->set_depth_locked(val);
}

void Control::start()
{
    m_isRunning = true;
    m_thread = std::thread([this]()
                           { this->run(); });
}

void Control::stop()
{
    m_isRunning = false;
    if (m_thread.joinable())
        m_thread.join();
}

void Control::addPWMDevice(PWMDevice::ptr device)
{
    m_pwmDevices.push_back(device);
}

const std::vector<PWMDevice::ptr> &Control::getPwmDevices() noexcept
{
    return m_pwmDevices;
}

void Control::run()
{
    while (m_isRunning)
    {
        m_propeller->run();

        for (auto &i : m_pwmDevices)
            i->run();


        static int pinBase = Global<PCA9685>::Get()->getPinBase();
        const int16_t *data = this->get6RawData();
        for (size_t i = 0; i < 6; ++i)
        {
            ::pwmWrite(pinBase + i, PCA9685::CalcTicks(data[i]));
        }

        auto devices = this->getPwmDevices();
        for (const auto &i : devices)
        {
            ::pwmWrite(pinBase + i->m_channel, PCA9685::CalcTicks(i->m_params.cur));
        }
    }
}
