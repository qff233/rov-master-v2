#include "method_get_info.h"

#include <iostream>

namespace Method 
{

void to_json(nlohmann::json &j, const Info& info) 
{
    j = nlohmann::json{
        {"温度", info.temperature},
        {"航向角", info.yaw},
        {"深度", info.depth}
    };
}

void from_json(const nlohmann::json &j, Info& info) {
    j.at("温度").get_to(info.temperature);
    j.at("航向角").get_to(info.yaw);
    j.at("深度").get_to(info.depth);
}

Info get_info() {
    std::cout << "get_info" << std::endl;
}

}