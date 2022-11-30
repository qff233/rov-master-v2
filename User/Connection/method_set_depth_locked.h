#include <easylogging++.h>
#include <nlohmann/json.hpp>

namespace Method 
{

Json set_depth_locked(bool val) 
{
    LOG(INFO) << "set_depth_locked" << val;
    return nullptr;
}

}