#include "method_get_feedbacks.h"
#include <iostream>

namespace Method 
{

void to_json(nlohmann::json &j, const FeedBack& feedback)
{
    j = nlohmann::json{
        {
            {"depth_lock", feedback.diffen_dep},
            {"direction_lock", feedback.yaw}
        }
    };
}

FeedBack get_feedback()
{
    std::cout << "FeedBack get_feedback()" << std::endl;
}

}