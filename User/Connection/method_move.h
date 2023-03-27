#include <nlohmann/json.hpp>

#include "User/utils.h"
#include "User/log.h"
#include "User/Control/control.h"

namespace Method 
{

using Json = nlohmann::json;

Json move(float x, float y, float z, float rot) 
{
    Global<Control>::Get()->move(x, y, z, rot, 0, 0);
    std::string msg = std::to_string(x*400) + ", " + std::to_string(y*400) + ", " + std::to_string(z*400) + "," + std::to_string(rot*400);
    LOG_INFO(msg);
    return nullptr;
}

Json move_absolute(float x, float y, float z, float rot)
{
    Global<Control>::Get()->move_absolute(x, y, z, rot);
    return nullptr;
}

Json move_relative(float x, float y, float z, float rot)
{
    Global<Control>::Get()->move_relative(x, y, z, rot);
    return nullptr;
}


} //namespace Metho
