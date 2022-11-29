#include "nlohmann/json.hpp"

// uint8_t cmd_data.depth_lock

namespace Method 
{

void to_json(nlohmann::json &j, const uint8_t& p);
void from_json(const nlohmann::json &j, uint8_t& p);

}