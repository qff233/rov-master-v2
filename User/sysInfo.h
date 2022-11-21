//
// Created by fxf on 22-11-21.
//

#ifndef _SYSINFO_H
#define _SYSINFO_H

#include "Drivers/jy901.h"

/* ROV设备 */
struct RovDev
{
    // 简单PWM设备
    EasyPWMDev yuntai;
    EasyPWMDev light;
    EasyPWMDev light2;
    EasyPWMDev robot_arm;
    // 推进器设备
    PropellerPower propellerPower;

};

enum class SysStatus 
{

};

/* ROV状态信息 */
struct RovInfo
{
    SysStatus type;                 // 系统状态
    std::string name[20];             // 航行器名称
    JY901 jy901;             // 九轴
    PowerSource powerSource; // 电源
    DepthSensor depthSensor; // 深度传感器

    Net net;       // 网卡
    Cpu cpu;       // cpu
    Disk disk;     // 硬盘
    Memory memory; // 内存
};

#endif //_SYSINFO_H
