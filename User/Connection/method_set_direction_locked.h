#include <easylogging++.h>
#include <nlohmann/json.hpp>

#include "User/utils.h"
#include "User/Control/control.h"

namespace Method 
{

using Json = nlohmann::json;

Json set_direction_locked(bool val) 
{
    LOG(INFO) << "set_direction_locked  " << val;
    Global<Control>::Get()->set_direction_locked(val);
    return nullptr;
}

}