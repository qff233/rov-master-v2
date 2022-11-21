/**
 * @desc: rpc协议所使用的方法函数
 */

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <wiringPi.h>

#include "../applications/data.h"
#include "../control/control.h"
#include "../drivers/pca9685.h"
#include "../user/datatype.h"
#include "cjson_anal.h"
#include "crpcfunc.h"

using namespace std;

/**
  * @brief  创建机器人状态信息json对象
  */
static cJSON* robort_status()
{
    cJSON* cjson_info = NULL;
    cjson_info = cJSON_CreateObject();
    char *temp_str = malloc(sizeof(char) * 20);
    sprintf(temp_str, "%.02f ℃", rovInfo.cpu.temperature);
    cJSON_AddStringToObject(cjson_info, "温度", temp_str);
    sprintf(temp_str, "%.02f °", rovInfo.jy901.yaw);
    cJSON_AddStringToObject(cjson_info, "航向角", temp_str);
    sprintf(temp_str, "%.02f cm", rovInfo.depthSensor.depth * 100.0f);
    cJSON_AddStringToObject(cjson_info, "深度", temp_str);
    free(temp_str);
    return cjson_info;
}

/**
  * @brief  创建机器人调试状态json对象
  */
static cJSON* robort_debug_status()
{
    cJSON* cjson_pid_feedbacks = NULL;
    cJSON* cjson_pid_control_loops = NULL;	
    cjson_pid_feedbacks = cJSON_CreateObject();
    cjson_pid_control_loops = cJSON_CreateObject();
    cJSON_AddNumberToObject(cjson_pid_control_loops, "depth_lock",diffen_dep);
    cJSON_AddNumberToObject(cjson_pid_control_loops, "direction_lock",rovInfo.jy901.yaw/360.0f);
    cJSON_AddItemToObject(cjson_pid_feedbacks, "control_loops", cjson_pid_control_loops);
    return cjson_pid_feedbacks;
}

/**
  * @brief  rov的info信息的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  info信息的json对象
  */
cJSON *info(jrpc_context *ctx, cJSON *params, cJSON *id)
{
  return robort_status();
} 

/**
  * @brief  debug的info信息的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  debug信息的json对象
  */
cJSON *debug_info(jrpc_context *ctx, cJSON *params, cJSON *id)
{
  return robort_debug_status();
}

/**
  * @brief  异步move控制的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  
  */
cJSON *move_asyn(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    return move_analysis(params, 0);
}

/**
  * @brief  同步move控制的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *move_syn(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    return move_analysis(params, 0);
}

/**
  * @brief  机械爪控制的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *catcher(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    arm_catch = params->child->valuedouble;
    return cJSON_CreateNull();
}

/**
  * @brief  深度控制的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  有返回表示抵达深度
  */
cJSON *depth(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    expect_depth_auv = params->child->valuedouble;
    return cJSON_CreateNull();
}

/**
  * @brief  到达指定航向角的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  有返回表示抵达方向
  */
cJSON *move_absolute(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    return move_analysis(params, 1);
}

/**
  * @brief  在当前航向角下旋转度数的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  有返回表示抵达方向
  */
cJSON *move_relative(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    return move_analysis(params, 2);
}

/**
  * @brief  定向控制的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *direction_lock(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    cmd_data.sport_lock = params->child->valueint;
    return cJSON_CreateNull();
}

/**
  * @brief  定深控制的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *depth_lock(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    cmd_data.depth_lock = params->child->valueint;
    return cJSON_CreateNull();
}

/**
  * @brief  调试模式标志的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *set_debug_mode_enabled(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    debug_mode_enable = params->child->valueint;
    return cJSON_CreateNull();
}

/**
  * @brief  pca9685频率参数的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *set_propeller_pwm_freq_calibration(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    cJSON_ReplaceItemInObject(propeller_parameters, "propeller_pwm_freq_calibration", cJSON_Parse(cJSON_PrintUnformatted(params->child)));
    pca9685_pwm = params->child->valuedouble;
    return cJSON_CreateNull();
}

/**
  * @brief  推进器输出的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *set_propeller_values(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    propeller_directer_value.back_left    = (short)cjson_value_analysis_int(params, "back_left");
    propeller_directer_value.back_right   = (short)cjson_value_analysis_int(params, "back_right");
    propeller_directer_value.center_left  = (short)cjson_value_analysis_int(params, "center_left");
    propeller_directer_value.center_right = (short)cjson_value_analysis_int(params, "center_right");
    propeller_directer_value.front_left   = (short)cjson_value_analysis_int(params, "front_left");    
    propeller_directer_value.front_right  = (short)cjson_value_analysis_int(params, "front_right");
    return cJSON_CreateNull();
}

/**
  * @brief  推进器参数的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *set_propeller_parameters(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    cJSON_ReplaceItemInObject(propeller_parameters, "propeller_parameters", cJSON_Parse(cJSON_PrintUnformatted(params)));
    propeller_parameter_analysis(params, &propeller_conpent_value.back_left   , "back_left");
    propeller_parameter_analysis(params, &propeller_conpent_value.back_right  , "back_right");
    propeller_parameter_analysis(params, &propeller_conpent_value.center_left , "center_left");
    propeller_parameter_analysis(params, &propeller_conpent_value.center_right, "center_right");
    propeller_parameter_analysis(params, &propeller_conpent_value.front_left  , "front_left");
    propeller_parameter_analysis(params, &propeller_conpent_value.front_right , "front_right"); 
    return cJSON_CreateNull();
}

/**
  * @brief  控制环参数的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *set_control_loop_parameters(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    if (params == NULL) return cJSON_CreateNull();
    cJSON_ReplaceItemInObject(propeller_parameters, "control_loop_parameters", cJSON_Parse(cJSON_PrintUnformatted(params)));
    PID_analysis(params, &pid_debug_arrey.direction_lock, "direction_lock");
    PID_analysis(params, &pid_debug_arrey.depth_lock, "depth_lock");
    return cJSON_CreateNull();
}

/**
  * @brief  保存推进器参数的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *save(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    static int pca_fd;
    save_propeller_file();
    pca_fd = wiringPiI2CSetupInterface(PCA9685_I2C_DEV, PCA9685_I2C_ADDR);
    if (pca_fd < 0) return cJSON_CreateNull();
    pca9685PWMSetFreq(pca_fd, 50);
    return cJSON_CreateNull();
}

/**
  * @brief  读取推进器参数的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *load(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    return load_propeller_file();
}

/**
  * @brief  固件更新的rpc函数
  * @param  *params json参数
  * @param  *id  客户端标识
  * @return  1：接收成功；0：接收失败
  */
cJSON *update(jrpc_context *ctx, cJSON *params, cJSON *id)
{
    return update_firmware(params);
}
