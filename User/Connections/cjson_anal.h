/*
 * @Author: your name
 * @Date: 2022-03-28 12:14:06
 * @LastEditTime: 2022-04-15 17:09:38
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \ROV2.3\drivers\cjson_anal.h
 */

#ifndef _CJSON_ANAL_H
#define _CJSON_ANAL_H

#include "../user/config.h"
#include "rpcdata.h"
#include "cjson.h"
#include "jsonrpc.h"

//固件更新定义
#define UPDATE_WAIT   2  //等待固件更新
#define UPDATING      1  //固件接收中
#define UPDATE_FINISH 0  //固件接收完毕
#define UPDATE_ERROR -1  //固件更新错误

cJSON *update_firmware(cJSON* params);
cJSON *move_analysis(cJSON* params, int mode);
int cjson_value_analysis_int(cJSON *params, const char *str);
void propeller_parameter_analysis(cJSON *params, cjson_set_propeller_parameters_t *compent, const char *str);
void PID_analysis(cJSON *fil, pid_debug_t *PID, const char *str);
void save_propeller_file(void);
cJSON *load_propeller_file(void);

#endif
