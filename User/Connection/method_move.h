#include <nlohmann/json.hpp>

#include "User/utils.h"
#include "User/Control/control.h"

namespace Method 
{

using Json = nlohmann::json;

Json move(float x, float y, float z, float rot) 
{
    Global<Control>::Get()->move(x, y, z, rot);
    LOG(INFO) << x*400 << ", " << y*400 << ", " << z*400 << "," << rot*400;
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
