#include "method_move.h"

#include <iostream>

namespace Method 
{

void to_json(nlohmann::json& json, const Motion& motion) 
{
    json = nlohmann::json{
        {"x", motion.x},
        {"y", motion.y},
        {"z", motion.z},
        {"rot", motion.rot}
    };
}

void from_json(const nlohmann::json& json, Motion& motion) 
{
    json.at("x").get_to(motion.x);
    json.at("y").get_to(motion.y);
    json.at("z").get_to(motion.z);
    json.at("rot").get_to(motion.rot);
}

void move(const Motion) 
{
    std::cout << "void move(const Motion) " << std::endl;
}

void move_absolute(const Motion) 
{
    std::cout << "void move_absolute(const Motion) " << std::endl;
}

void move_relative(const Motion) 
{
    std::cout << "void move_relative(const Motion) " << std::endl;
}


} //namespace Metho
