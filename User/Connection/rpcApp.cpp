//
// Created by fxf on 22-11-21.
//

#include <string>

#include "rpcApp.h"

using namespace std;

bool RPCAPP::info(const Json::Value& root, Json::Value& response)
{
    std::string temp = " ℃";
    temp = std::to_string(rovInfo.cpu.temperature) + temp;
    response["温度"] = Json::Value(temp);
    return true;
}

bool RPCAPP::debug_info(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::move_asyn(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::move_syn(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::catcher(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::depth(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::move_absolute(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::move_relative(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::direction_lock(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::depth_lock(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::set_debug_mode_enabled(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::set_propeller_pwm_freq_calibration(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::set_propeller_values(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::set_propeller_parameters(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::set_control_loop_parameters(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::save(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::load(const Json::Value& root, Json::Value& response)
{
    return true;
}

bool RPCAPP::update(const Json::Value& root, Json::Value& response)
{
    return true;
}
