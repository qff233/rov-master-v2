#include "nlohmann/json.hpp"

namespace Method 
{

struct Info 
{
    float temperature;
    float yaw;
    float depth;
};

void to_json(nlohmann::json &j, const Info& p);
void from_json(const nlohmann::json &j, Info& p);

Info get_info();

}