#include "control.h"

#include <chrono>
#include <wiringPi.h>
#include <pca9685.h>

#include "control_propeller.h"
#include "pwm_devices.h"
#include "User/utils.h"

using namespace std::chrono_literals;

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

void Control::move(float rocker_x, float rocker_y, float rocker_z, float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept
{
    m_propeller->move(rocker_x, rocker_y, rocker_z, rocker_yaw, rocker_roll, rocker_pitch);
    m_cond_var.notify_one();
}

void Control::move_absolute(float rocker_x, float rocker_y, float rocker_z, float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept
{
    m_propeller->move_absolute(rocker_x, rocker_y, rocker_z, rocker_yaw, rocker_roll, rocker_pitch);
    m_cond_var.notify_one();
}

void Control::move_relative(float rocker_x, float rocker_y, float rocker_z, float rocker_yaw, float rocker_roll, float rocker_pitch) noexcept
{
    m_propeller->move_relative(rocker_x, rocker_y, rocker_z, rocker_yaw, rocker_roll, rocker_pitch);
    m_cond_var.notify_one();
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

    m_cond_var.notify_one();
}

void Control::to_depth(float depth) noexcept
{
    m_propeller->to_depth(depth);
    m_cond_var.notify_one();
}

void Control::set_direction_locked(float val) noexcept
{
    m_propeller->set_direction_locked(val);
    m_cond_var.notify_one();
}

void Control::set_depth_locked(float val) noexcept
{
    m_propeller->set_depth_locked(val);
    m_cond_var.notify_one();
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
    {
        m_thread.join();
        m_cond_var.notify_one();
    }
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
    std::unique_lock<std::mutex> lk(m_mutex);

    while (m_isRunning)
    {
        m_cond_var.wait_for(lk, 50ms);

        m_propeller->run();

        for (auto &i : m_pwmDevices)
            i->run();


        static auto* pca9685 = Global<PCA9685>::Get();
        static int pinBase = pca9685->getPinBase();

        const int16_t *data = this->get6RawData();
        for (size_t i = 0; i < 6; ++i)
        {
            pca9685->pwmWrite(pinBase + i, PCA9685::CalcTicks(data[i]));
        }

        auto devices = this->getPwmDevices();
        for (const auto &i : devices)
        {
            pca9685->pwmWrite(pinBase + i->m_channel, PCA9685::CalcTicks(i->m_params.cur));
        }
    }
}
