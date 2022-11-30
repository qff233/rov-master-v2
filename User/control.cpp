#include "control.h"

const int16_t* ControlBase::get6RawData() noexcept {
    return (int16_t*)&m_propellerPower;
}

std::array<int16_t, 4> ControlBase::parase_rocker(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) 
{
    return {(int16_t(rocker_x), int16_t(rocker_y), int16_t(rocker_z), int16_t(rocker_rot))};
}

void ControlV2::move(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept 
{
    auto[x, y, z, rot] = this->parase_rocker(rocker_x, rocker_y, rocker_z, rocker_rot);
    
}

void ControlV2::move_absolute(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept 
{
    auto[x, y, z, rot] = this->parase_rocker(rocker_x, rocker_y, rocker_z, rocker_rot);
}
void ControlV2::move_relative(float rocker_x, float rocker_y, float rocker_z, float rocker_rot) noexcept 
{
    auto[x, y, z, rot] = this->parase_rocker(rocker_x, rocker_y, rocker_z, rocker_rot);
}
