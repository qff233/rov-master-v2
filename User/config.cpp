#include "config.h"

#include <fstream>

#include "Control/control.h"
#include "Control/pwm_devices.h"
#include "Control/control_propeller.h"

// void Config::load_from_path(const std::string& path) noexcept
// {

// }

void to_json(Json &j, const PIDParameters &p)
{
    j = Json{
        {"p", p.p},
        {"i", p.i},
        {"d", p.d}};
}

void from_json(const Json &j, PIDParameters &p)
{
    j.at("p").get_to(p.p);
    j.at("i").get_to(p.i);
    j.at("d").get_to(p.d);
}

void to_json(Json &j, const ControlLoopParameters &p)
{
    j = Json{
        {"depth_lock", p.depthLock},
        {"direction_lock", p.directionLock}};
}

void from_json(const Json &j, ControlLoopParameters &p)
{
    j.at("depth_lock").get_to(p.depthLock);
    j.at("direction_lock").get_to(p.directionLock);
}

void to_json(Json &j, const PropellerParameters &p)
{
    j = Json{
        {"propeller_pwm_freq_calibration", p.pwmFreqCalibration},
        {"propeller_parameters", p.propellerGroup},
        {"control_loop_parameters", p.controlLoopParameters}};
}

void from_json(const Json &j, PropellerParameters &p)
{
    j.at("propeller_pwm_freq_calibration").get_to(p.pwmFreqCalibration);
    j.at("propeller_parameters").get_to(p.propellerGroup);
    j.at("control_loop_parameters").get_to(p.controlLoopParameters);
}

void to_json(Json &j, const PropellerAttribute &p)
{
    j = Json{
        {"deadzone_lower", p.deadZoneLower},
        {"deadzone_upper", p.deadZoneUpper},
        {"power_positive", p.powerPositive},
        {"power_negative", p.powerNegative},
        {"reversed", p.reversed},
        {"enabled", p.enabled}};
}

void from_json(const Json &j, PropellerAttribute &p)
{
    j.at("deadzone_lower").get_to(p.deadZoneLower);
    j.at("deadzone_upper").get_to(p.deadZoneUpper);
    j.at("power_positive").get_to(p.powerPositive);
    j.at("power_negative").get_to(p.powerNegative);
    j.at("reversed").get_to(p.reversed);
    j.at("enabled").get_to(p.enabled);
}

void to_json(Json &j, const PropellerGroup &p)
{
    j = Json{
        {"back_right", p.backRight},
        {"center_right", p.middleRight},
        {"front_right", p.frontRight},
        {"center_left", p.middleLeft},
        {"front_left", p.frontLeft},
        {"back_left", p.backLeft}};
}

void from_json(const Json &j, PropellerGroup &p)
{
    j.at("back_right").get_to(p.backRight);
    j.at("center_right").get_to(p.middleRight);
    j.at("front_right").get_to(p.frontRight);
    j.at("center_left").get_to(p.middleLeft);
    j.at("front_left").get_to(p.frontLeft);
    j.at("back_left").get_to(p.backLeft);
}

void to_json(Json &j, const PWMDeviceParams &p)
{
    j = Json{
        {"cur", p.cur},
        {"mid", p.mid},
        {"pMax", p.pMax},
        {"nMax", p.nMax},
        {"speed", p.speed}};
}

void from_json(const Json &j, PWMDeviceParams &p)
{
    j.at("cur").get_to(p.cur);
    j.at("mid").get_to(p.mid);
    j.at("pMax").get_to(p.pMax);
    j.at("nMax").get_to(p.nMax);
    j.at("speed").get_to(p.speed);
}

void Config::load_from_file(const std::string &file_path) noexcept
{
    std::fstream fs(file_path);
    auto old_json = m_datas;
    fs >> m_datas;
    for (auto &i : m_cbs)
    {
        auto tmp_old = m_datas.at(i.first);
        auto tmp_new = m_datas.at(i.first);
        i.second(tmp_old, tmp_new);
    }
}

std::string Config::dump() const noexcept
{
    return m_datas.dump();
}
