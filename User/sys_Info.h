//
// Created by fxf on 22-11-21.
//

#ifndef _SYSINFO_H
#define _SYSINFO_H

#include "Drivers/jy901.h"
#include "Drivers/ms5837.h"

/* ROV状态信息 */
struct RovInfo
{
    SysStatus type;          // 系统状态
    std::string name[20];    // 航行器名称
    JY901 jy901;             // 九轴
    PowerSource powerSource; // 电源
    Ms5837Data depthSensor;  // 深度传感器

    Net net;       // 网卡
    Cpu cpu;       // cpu
    Disk disk;     // 硬盘
    Memory memory; // 内存
};

struct HostInfo
{
    pid_debug_arrey_t pid_debug_arrey;
    propeller_conpent_t propeller_conpent_value;
    propeller_directer_value_t propeller_directer_value;
    // cJSON *propeller_parameters;
    rocket_raw_t rocket;
    float expect_depth_auv;
    float expect_rotate_auv;
    uint8_t debug_mode_enable;
    float arm_catch;
    cmd_t cmd_data;
};

#endif //_SYSINFO_H
