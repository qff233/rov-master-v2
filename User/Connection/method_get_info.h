#include "nlohmann/json.hpp"

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
    info.depth = 111.12;
    info.temperature = 232.13;
    info.yaw = 21.23;
    return info;
}

}