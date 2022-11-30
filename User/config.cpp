#include "config.h"

#include <fstream>

// void Config::load_from_path(const std::string& path) noexcept
// {

// }

void to_json(nlohmann::json &j, const PropellerParameters& p)
{
    j = Json {
        {"propeller_pwm_freq_calibration", p.pwmFreqCalibration},
        {"propeller_parameters", p.propellerGroup}
    };
}

void from_json(const nlohmann::json &j, PropellerParameters& p)
{
    j.at("propeller_pwm_freq_calibration").get_to(p.pwmFreqCalibration);
    j.at("propeller_parameters").get_to(p.propellerGroup);
}

void to_json(nlohmann::json &j, const PropellerGroup& p)
{
    j = Json {
        {"back_right", p.backRight},
        {"center_right", p.middleRight},
        {"front_right", p.frontRight},
        {"center_left", p.middleLeft},
        {"front_left", p.frontLeft},
        {"back_left", p.backLeft}
    };
}

void from_json(const nlohmann::json &j, PropellerGroup& p)
{
    j.at("back_right").get_to(p.backRight);
    j.at("center_right").get_to(p.middleRight);
    j.at("front_right").get_to(p.frontRight);
    j.at("center_left").get_to(p.middleLeft);
    j.at("front_left").get_to(p.frontLeft);
    j.at("back_left").get_to(p.backLeft);
}

void to_json(nlohmann::json &j, const PropellerAttribute& p)
{
    j = Json {
        {"deadzone_lower", p.deadZoneLower},
        {"deadzone_upper", p.deadZoneUpper},
        {"power_positive", p.powerPositive},
        {"power_negative", p.powerNegative},
        {"reversed", p.reversed},
        {"enabled", p.enabled}
    };
}

void from_json(const nlohmann::json &j, PropellerAttribute& p)
{
    j.at("deadzone_lower").get_to(p.deadZoneLower);
    j.at("deadzone_upper").get_to(p.deadZoneUpper);
    j.at("power_positive").get_to(p.powerPositive);
    j.at("power_negative").get_to(p.powerNegative);
    j.at("reversed").get_to(p.reversed);
    j.at("enabled").get_to(p.enabled);
}

void Config::load_from_file(const std::string &file_path) noexcept
{
    std::fstream fs(file_path);
    fs >> m_datas;
}

std::string Config::dump() const noexcept
{
    return m_datas.dump();
}
