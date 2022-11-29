#include "method_get_info.h"

namespace Method 
{

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

void from_json(const nlohmann::json &j, Info& info) {
    j.at("温度").get_to(info.temperature);
    j.at("航向角").get_to(info.yaw);
    j.at("深度").get_to(info.depth);
}

Info get_info() {
    Info info;
    info.depth = 111.12;
    info.temperature = 232.13;
    info.yaw = 21.23;
    return info;
}

}