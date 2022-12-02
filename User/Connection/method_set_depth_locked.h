#include <easylogging++.h>
#include <nlohmann/json.hpp>

#include "User/utils.h"
#include "User/Control/control.h"

namespace Method 
{

Json set_depth_locked(bool val) 
{
    LOG(INFO) << "set_depth_locked  " << val;
    Global<Control>::Get()->set_depth_locked(val);
    return nullptr;
}


}