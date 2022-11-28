#include "nlohmann/json.hpp"

namespace Method 
{

struct Motion 
{
    float x;
    float y;
    float z;
    float rot;
};
void to_json(nlohmann::json &j, const Motion& p);
void from_json(const nlohmann::json &j, Motion& p);


void move(const Motion);
void move_absolute(const Motion);
void move_relative(const Motion);


} //namespace Metho
