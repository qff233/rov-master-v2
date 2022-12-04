#include <nlohmann/json.hpp>

#include "User/utils.h"
#include "User/log.h"
#include "User/Control/control.h"

namespace Method 
{

Json set_depth_locked(bool val) 
{
    std::string ms = "set_depth_locked  " + std::to_string(val);
    LOG_INFO(ms);
    Global<Control>::Get()->set_depth_locked(val);
    return nullptr;
}


}