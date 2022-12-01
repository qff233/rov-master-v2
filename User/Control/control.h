#ifndef __USER_CONTROL_H__
#define __USER_CONTROL_H__

#include <thread>

#include "pwm_devices.h"
#include "Control/control_propeller.h"

class Control
{
public:
    Control(PropellerControlBase::ptr&& propellerControl);
    ~Control();

    void move(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept;
    void move_absolute(float rocker_x, float rocker_y, float rocker_z, float rot) noexcept;
    void move_relative(float rocker_x, float rocker_y, float rocker_z, float rot) noexcept;
    void catcher(float val) noexcept;
    void to_depth(float depth) noexcept;
    void set_direction_locked(float val) noexcept;
    void set_depth_locked(float val) noexcept;

    void start();
    void stop();

    void addPWMDevice(PWMDevice::ptr &&device);
private:
    const std::vector<PWMDevice::ptr> &getPwmDevices() noexcept;
    void run();
private:
    std::thread m_thread;
    bool m_isRunning = false;
    PropellerControlBase::ptr m_propeller;
    std::vector<PWMDevice::ptr> m_pwmDevices;
};

#endif