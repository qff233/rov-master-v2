//
// Created by fxf on 22-11-21.
//

#ifndef _RPCDATA_H
#define _RPCDATA_H

#include "cjson.h"

typedef struct
{
    int     deadzone_upper;    // 死区上限
    int     deadzone_lower;    // 死区下限
    double  power_positive;    // 动力百分比
    double  power_negative;    // 动力百分比
    int     reversed;          // 反转
    int     reversed_flag;     // 反转
    int     enabled;           // 启用/禁用推进器
}cjson_set_propeller_parameters_t;

//推进器参数数据结构，需要增加推进器 加该数据结构中
typedef struct
{
    cjson_set_propeller_parameters_t back_left;     // 左后
    cjson_set_propeller_parameters_t back_right;    //..
    cjson_set_propeller_parameters_t center_left;   //..
    cjson_set_propeller_parameters_t center_right;  //..
    cjson_set_propeller_parameters_t front_left;    //..
    cjson_set_propeller_parameters_t front_right;   //..

}propeller_conpent_t;

/*     PID 调试参数      */
typedef struct
{
    double p;
    double i;
    double d;

}pid_debug_t;

/*     总 PID 调试参数      */
typedef struct
{
    pid_debug_t direction_lock;
    pid_debug_t depth_lock;

}pid_debug_arrey_t;


/*调试模式推进器直接输出结构体*/
typedef struct
{
    short front_left;
    short front_right;
    short back_left;
    short back_right;
    short center_left;
    short center_right;

}propeller_directer_value_t;

/*原始手柄数据结构体*/
typedef struct cjson_test
{
    double x;  //左右
    double y;  //前后
    double z;  //上下
    double yaw;

}rocket_raw_t;

typedef struct
{
    /* 开关类 */
    uint8_t all_lock;   //【总开关】 0x01锁定(默认)、0x00解锁
    uint8_t depth_lock; //【深度】	 0x01 深度锁定、0x00 人工控制
    uint8_t sport_lock; //【运动】	 0x01 方向锁定、0x00 随水动

    /* 设备类 */
    uint8_t power;  //【油门】	0x00~0xff (0~255) 表示的油门大小：4档位可调，LB加档，LT减档，可分别设置4个档位油门大小
    uint8_t light;  //【灯光】	0x01 打开、0x00 关闭（默认）
    uint8_t camera; //【聚焦】  0x01 聚焦、0x02 放焦、  0x11 放大、0x12 缩小、0x00表示不动作（默认）
    uint8_t yuntai; //【云台】	0x01 向上、0x02 向下、  0x00表示不动作（默认）
    uint8_t arm;    //【机械臂】0x01 张开、0x02 关闭、  0x00表示不动作（默认）

} cmd_t; /* 控制命令 */

extern pid_debug_arrey_t pid_debug_arrey;
extern propeller_conpent_t propeller_conpent_value;
extern propeller_directer_value_t propeller_directer_value;
extern cJSON *propeller_parameters;
extern rocket_raw_t rocket;
extern float expect_depth_auv;
extern float expect_rotate_auv;
extern uint8_t debug_mode_enable;
extern float arm_catch;
extern cmd_t cmd_data;

#endif //_RPCDATA_H
