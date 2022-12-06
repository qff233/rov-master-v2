#ifndef __USER_CONTROL_H__
#define __USER_CONTROL_H__

#include <thread>
#include <vector>
#include <memory>
#include <semaphore>

class PropellerControlBase;
class PWMDevice;

class EventPCA9685;

class Control
{
public:
    friend EventPCA9685;

    Control(std::unique_ptr<PropellerControlBase> &&propellerControl);
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

    void addPWMDevice(std::shared_ptr<PWMDevice> device);

private:
    const int16_t *get6RawData() noexcept; // 注意 长度为6
    const std::vector<std::shared_ptr<PWMDevice>> &getPwmDevices() noexcept;
    void run();

private:
    std::thread m_thread;
    std::binary_semaphore m_moveSemaphore{0};
    bool m_isRunning = false;
    std::unique_ptr<PropellerControlBase> m_propeller;
    std::vector<std::shared_ptr<PWMDevice>> m_pwmDevices;
};

#endif