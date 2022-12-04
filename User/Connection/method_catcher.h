#include <nlohmann/json.hpp>

#include "User/log.h"

namespace Method 
{

using Json = nlohmann::json;

Json catcher(float val) 
{
    std::string ms = "catcher " + std::to_string(val);
    LOG_INFO(ms);
    return nullptr;
}

}