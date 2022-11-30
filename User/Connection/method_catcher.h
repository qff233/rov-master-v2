#include <easylogging++.h>
#include <nlohmann/json.hpp>

namespace Method 
{

using Json = nlohmann::json;

Json catcher(float val) 
{
    LOG(INFO) << "catcher" << val;
    return nullptr;
}

}