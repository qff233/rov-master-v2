#include "nlohmann/json.hpp"

#include "User/utils.h"
#include "jy901.h"
#include "ms5837.h"

namespace Method 
{

struct Info 
{
    float temperature;
    float yaw;
    float depth;
};

void to_json(nlohmann::json &j, const Info& info) 
{
    std::string temperature{std::to_string(info.temperature)};
    std::string yaw{std::to_string(info.yaw)};
    std::string depth{std::to_string(info.depth)};
    j = nlohmann::json{
        {"温度", temperature.substr(0, temperature.size()-4)+"℃"},
        {"航向角", yaw.substr(0, yaw.size()-4)+"°"},
        {"深度", depth.substr(0, depth.size()-4)+"cm"}
    };
}

Info get_info()
{
    Info info;
    info.depth = Global<MS5837>::Get()->getDepth();
    info.temperature = Global<MS5837>::Get()->getTemperature();
    info.yaw = Global<JY901>::Get()->getYaw();
    return info;
}

}