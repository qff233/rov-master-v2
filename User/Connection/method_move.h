#include <nlohmann/json.hpp>

namespace Method 
{

using Json = nlohmann::json;

Json move(float x, float y, float z, float rot) 
{
    LOG(INFO) << x << ", " << y << ", " << z << "," << rot;
    return nullptr;
}

Json move_absolute(float x, float y, float z, float rot)
{
    return nullptr;
}

Json move_relative(float x, float y, float z, float rot)
{
    return nullptr;
}


} //namespace Metho
