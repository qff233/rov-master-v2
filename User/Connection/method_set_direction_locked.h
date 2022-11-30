#include <easylogging++.h>
#include <nlohmann/json.hpp>


namespace Method 
{

using Json = nlohmann::json;

Json set_direction_locked(bool val) 
{
    LOG(INFO) << "set_direction_locked" << val;
    return nullptr;
}

}