#include "nlohmann/json.hpp"

// float expect_depth_auv

namespace Method 
{

void to_json(nlohmann::json &j, const float& p);
void from_json(const nlohmann::json &j, float& p);

}