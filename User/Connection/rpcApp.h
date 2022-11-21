//
// Created by fxf on 22-11-21.
//

#ifndef _RPCAPP_H
#define _RPCAPP_H

#include <jsoncpp/json.h>

class RPCAPP
{
public:
    bool info(const Json::Value& root, Json::Value& response);
    bool debug_info(const Json::Value& root, Json::Value& response);
    bool move_asyn(const Json::Value& root, Json::Value& response);
    bool move_syn(const Json::Value& root, Json::Value& response);
    bool catcher(const Json::Value& root, Json::Value& response);
    bool depth(const Json::Value& root, Json::Value& response);
    bool move_absolute(const Json::Value& root, Json::Value& response);
    bool move_relative(const Json::Value& root, Json::Value& response);
    bool direction_lock(const Json::Value& root, Json::Value& response);
    bool depth_lock(const Json::Value& root, Json::Value& response);
    bool set_debug_mode_enabled(const Json::Value& root, Json::Value& response);
    bool set_propeller_pwm_freq_calibration(const Json::Value& root, Json::Value& response);
    bool set_propeller_values(const Json::Value& root, Json::Value& response);
    bool set_propeller_parameters(const Json::Value& root, Json::Value& response);
    bool set_control_loop_parameters(const Json::Value& root, Json::Value& response);
    bool save(const Json::Value& root, Json::Value& response);
    bool load(const Json::Value& root, Json::Value& response);
    bool update(const Json::Value& root, Json::Value& response);
};

#endif //_RPCAPP_H
