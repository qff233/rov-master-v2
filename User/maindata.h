//
// Created by fxf on 22-11-21.
//

#ifndef _MAINDATA_H
#define _MAINDATA_H

/* ROV设备 */
typedef struct
{
    // 简单PWM设备
    easyPWM_dev_t yuntai;
    easyPWM_dev_t light;
    easyPWM_dev_t light2;
    easyPWM_dev_t robot_arm;
    // 推进器设备
    propellerPower_t propellerPower;

} rovDev_t;

/* ROV状态信息结构体 */
typedef struct
{
    char type;                 // 系统状态
    char name[20];             // 航行器名称
    jy901_t jy901;             // 九轴
    powerSource_t powerSource; // 电源
    depthSensor_t depthSensor; // 深度传感器

    net_t net;       // 网卡
    cpu_t cpu;       // cpu
    disk_t disk;     // 硬盘
    memory_t memory; // 内存

} rovInfo_t;

extern rovDev_t rovdev;
extern rovInfo_t rovInfo;

#endif //_MAINDATA_H
