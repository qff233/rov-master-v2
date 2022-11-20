#define LOG_TAG "Init"

#include <cstring>
#include <cerrno>
#include <elog.h>
#include <wiringPi.h>

#include "init.h"

void system_init()
{
    easylogger_init(); // easylogger日志系统 初始化
    
    load_propeller_file();   //推进器及PID参数初始化

    if (wiringPiSetup() < 0) //判断NanoPi 是否运行成功
    {
        log_e("Unable to start wiringPi: %s", strerror(errno));
        return;
    }
    log_i("Welcome to ROV Master V%s\n", ROV_MASTER_VERSION);

    rov_all_params_init(); // ROV参数初始化

    sensor_thread_init();  // 传感器线程 初始化

    pwmDevs_thread_init(); // 外挂PWM设备线程 初始化

    ioDevs_thread_init();  // IO设备线程 初始化

    system_status_thread_init(); // 获取系统状态线程 初始化

    propeller_thread_init(); // 推进器线程 初始化
    
    control_server_thread_init(); //RPC线程 初始化
}