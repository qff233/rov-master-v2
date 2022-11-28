#include "nlohmann/json.hpp"

namespace Method 
{

struct FeedBack 
{
    float diffen_dep;
    float yaw;
};

void to_json(nlohmann::json &j, const FeedBack& feedback);

FeedBack get_feedback();

}