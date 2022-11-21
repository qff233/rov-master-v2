/**
 * @desc: rpc方法函数参数解析
 */

#define LOG_TAG "rpc"

#include <stdio.h>
#include <stdlib.h>
#include <elog.h>
#include <unistd.h> 
#include <string.h>

#include "cjson_anal.h"
#include "../drivers/pca9685.h"
#include "../control/PID.h"
#include "../control/control.h"
#include "../user/datatype.h"


float expect_depth_auv;   //上位机期望深度
float expect_rotate_auv;  //上位机期望转动方向
uint8_t debug_mode_enable;//调试模式
float arm_catch;          //机械臂控制
rocket_raw_t rocket;          //手柄数据结构体
propeller_conpent_t propeller_conpent_value; //推进器死区数据结构
pid_debug_arrey_t pid_debug_arrey;           //PID调节数组
propeller_directer_value_t propeller_directer_value = {0, 0, 0, 0, 0, 0}; //推进器输出
cJSON *propeller_parameters = NULL;          //存放推进器参数的json对象

// 解码时使用的   base64DecodeChars
static const unsigned char base64_suffix_map[256] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255,
        255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
        52 ,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
        255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
        7  ,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
        19 ,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
        255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
        37 ,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
        49 ,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255 };

/* 上位机的控制数据 */
cmd_t cmd_data = {
    .all_lock = CONTROL_LOCK
};

/**
 * @brief  int型数据处理函数
 * @param  *params json对象(类字典)
 * @param  *str json对象中的key
 * @return 解析收的int型数据(默认为0)
 */
int cjson_value_analysis_int(cJSON *params, const char *str)
{
    cJSON* cjson_temp = NULL;
    int value;
    cjson_temp = cJSON_GetObjectItem(params, str);
    if (cjson_temp != NULL)      //如果没收到则返回
    {
        value = cjson_temp->valueint;
    }
    else value = 0;
    return value;
}

/**
 * @brief  double型数据处理函数
 * @param  *params json对象(类字典)
 * @param  *str json对象中的key
 * @return 解析后的double型数据(默认为0)
 */
double cjson_value_analysis_double(cJSON *params,const char *str)
{
    cJSON* cjson_temp = NULL;
    double value;
    cjson_temp = cJSON_GetObjectItem(params, str);
    if (cjson_temp != NULL)      //如果没收到则返回
    {
        value = cjson_temp->valuedouble;
    }
    else value = 0;
    return value;
}

/**
 * @brief  move指令参数解析函数
 * @param  *params 上位机传过来的params参数
 * @param  mode 异步同步标志   
 * @return json的null
 */
cJSON *move_analysis(cJSON* params, int mode)
{
    rocket.x = cjson_value_analysis_double(params, "x") * 127;
    rocket.y = cjson_value_analysis_double(params, "y") * 127;
    rocket.z = cjson_value_analysis_double(params, "z") * 127;
    switch (mode)
    {
        case 0:
            rocket.yaw = cjson_value_analysis_double(params, "rot") * 127;
            break; 
        case 1:
            rocket.yaw = 0;  
            Total_Controller.Yaw_Angle_Control.Expect = -cjson_value_analysis_double(params, "rot");
            break;
        case 2:
            rocket.yaw = 0; 
            expect_rotate_auv = -cjson_value_analysis_double(params, "rot");
            break;
    }
    return cJSON_CreateNull();
}

/**
 * @brief  推进器参数数据处理函数
 * @param  *params json对象(类字典)
 * @param  *compent  目标推进器
 * @param  *str json对象中的key
 */
void propeller_parameter_analysis(cJSON *params, cjson_set_propeller_parameters_t *compent, const char *str)
{
    cJSON* cjson_temp = NULL;
    cjson_temp = cJSON_GetObjectItem(params, str);
    if (cjson_temp != NULL )
    {
        compent->deadzone_upper = cjson_value_analysis_int(cjson_temp, "deadzone_upper");
        compent->deadzone_lower = cjson_value_analysis_int(cjson_temp, "deadzone_lower");
        compent->enabled        = cjson_value_analysis_int(cjson_temp, "enabled"); 
        compent->reversed_flag  = cjson_value_analysis_int(cjson_temp, "reversed");
        compent->power_negative = cjson_value_analysis_double(cjson_temp, "power_negative");
        compent->power_positive = cjson_value_analysis_double(cjson_temp, "power_positive");
        if (compent->reversed_flag == 0) compent->reversed = 1;
        else if (compent->reversed_flag == 1) compent->reversed = -1;
    }
}

/**
 * @brief  PID参数数据处理函数
 * @param  *params json对象(类字典)
 * @param  *PID 目标pid
 * @param  *str json对象中的key
 */
void PID_analysis(cJSON *params, pid_debug_t *PID, const char *str)
{
    cJSON* cjson_temp = NULL;
    cjson_temp = cJSON_GetObjectItem(params, str);
    if (cjson_temp != NULL )
    {
        PID->p = cjson_value_analysis_double(cjson_temp, "p");
        PID->i = cjson_value_analysis_double(cjson_temp, "i");
        PID->d = cjson_value_analysis_double(cjson_temp, "d");
    }
    // 实时更新对应PID参数
    if (!strcmp(str, "direction_lock"))
    {
        Total_Controller.Roll_Angle_Control.Kp = (float)PID->p;
        Total_Controller.Roll_Angle_Control.Ki = (float)PID->i;
        Total_Controller.Roll_Angle_Control.Kd = (float)PID->d;
    }
    else 
    {
        Total_Controller.High_Position_Control.Kp = (float)PID->p;
        Total_Controller.High_Position_Control.Ki = (float)PID->i;
        Total_Controller.High_Position_Control.Kd = (float)PID->d;
    }
}

/**
 * @brief  保存推进器参数
 */
void save_propeller_file(void)
{
    FILE *fp;
    char fileName[] = "./data/propeller_parameter.txt";  //保存在data文件夹下
    //使用“写入”方式打开文件
    fp = fopen(fileName, "w");
    //如果文件不存在
    if (fp == NULL)
    {
        printf("no have parameter file ");
        //使用“写入”方式创建文件
        fp = fopen(fileName, "wt+");
        printf("creating and writing file.............");
    }
    fputs(cJSON_PrintUnformatted(propeller_parameters), fp);   //写入文件
    log_i("save parameters success!");
    fclose(fp); //关闭文件
}

/**
 * @brief  读取和解析推进器数据参数
 * @return 读取参数的json对象
 */
cJSON *load_propeller_file(void)
{   
    char buf[2048] = {0};
    char *temp = buf;
    FILE *fp;
    char fileName[] = "./data/propeller_parameter.txt";  //读取在data文件夹下
    //使用“读取”方式打开文件
    fp = fopen(fileName, "r");
    //如果文件不存在
    if (fp == NULL)
    {
        printf("no have parameter file ");
        //使用“写入”方式创建文件
        fp = fopen(fileName, "wt+");
        printf("creating and writing file.............");
    }
    while(fgets(buf, 2048, fp) != NULL)
    {
       // printf("%s\n", buf);
    }
    propeller_parameters = cJSON_Parse(buf);
    load_propeller_parameters(propeller_parameters);  //解析数据；
    fclose(fp); //关闭文件
    return cJSON_Parse(buf);
}

/**
 * @brief  将params参数替换obj中对应内容
 * @param  *obj 存入json对象
 * @param  *str 替换的参数名  
 * @param  *params 获取的新参数  
 */
void cjson_replace_params(cJSON *obj, const char *str, cJSON *params)
{
    cJSON *new = cJSON_Parse(cJSON_PrintUnformatted(params));
    cJSON_ReplaceItemInObject(propeller_parameters, str, cJSON_Parse(cJSON_PrintUnformatted(params)));
}

/**
 * @brief  加载保存参数函数
 * @param  *params 文件中保存的params参数   
 */
void load_propeller_parameters(cJSON* params)
{    
    cJSON* cjson_set_propeller_parameters = NULL; //推进器设置
    cJSON* set_control_loop_parameters = NULL;    //PID解析
    if (params == NULL)
    {
        log_e("load error");
        return 0;
    }
    /*pca9685频率设置*/
    pca9685_pwm = (float)cjson_value_analysis_double(params, "propeller_pwm_freq_calibration");
    /*推进器参数设置*/
    cjson_set_propeller_parameters = cJSON_GetObjectItem(params, "propeller_parameters");
    if (cjson_set_propeller_parameters != NULL)
    {    
        /*数据处理*/
        propeller_parameter_analysis(cjson_set_propeller_parameters, &propeller_conpent_value.back_left   , "back_left");
        propeller_parameter_analysis(cjson_set_propeller_parameters, &propeller_conpent_value.back_right  , "back_right");
        propeller_parameter_analysis(cjson_set_propeller_parameters, &propeller_conpent_value.center_left , "center_left");
        propeller_parameter_analysis(cjson_set_propeller_parameters, &propeller_conpent_value.center_right, "center_right");
        propeller_parameter_analysis(cjson_set_propeller_parameters, &propeller_conpent_value.front_left  , "front_left");
        propeller_parameter_analysis(cjson_set_propeller_parameters, &propeller_conpent_value.front_right , "front_right");         
    }
    /*PID解析*/
    set_control_loop_parameters= cJSON_GetObjectItem(params, "control_loop_parameters");
    if (set_control_loop_parameters != NULL) 
    {
        PID_analysis(set_control_loop_parameters,&pid_debug_arrey.depth_lock, "depth_lock");
        PID_analysis(set_control_loop_parameters,&pid_debug_arrey.direction_lock, "direction_lock");
    }
    log_i("load parameters success:");
    printf("%s\n\n", cJSON_Print(params));
}

/**
 * @brief  解码base64字符串
 * @param  *indata 输入的需要解码的base64字符串
 * @param  inlen 输入的需要解码的base64字符串的长度
 * @param  *outdata 存放解码后字符串的数组
 * @param  *outlen 解码后字符串长度
 * @param  countlen 当前已经接收到的base64长度
 * @return 0：解码成功；-1：空输入/无输出目标；-2：base64字符串不是四的倍数
 */
static int base64_decode(const char *indata, uint16_t inlen, char *outdata, uint16_t *outlen, uint64_t countlen) 
{    
    int ret = 0;
    if (indata == NULL || inlen <= 0 || outdata == NULL || outlen == NULL || countlen <= 0) {
        return ret = -1;
    }
    if (inlen % 4 != 0) { // 需要解码的数据不是4字节倍数
        return ret = -2;
    }

    int t = 0, x = 0, y = 0;
    uint64_t i = countlen;
    unsigned char c = 0;
    int g = 3;
    
    while (x < inlen) {
        // 需要解码的数据对应的ASCII值对应base64_suffix_map的值
        c = base64_suffix_map[indata[x++]];
        if (c == 255) return -1;     // 对应的值不在转码表中
        if (c == 253) continue;      // 对应的值是换行或者回车
        if (c == 254) { c = 0; g--; }// 对应的值是'='
        t = (t << 6) | c;              // 将其依次放入一个int型中占3字节
        if (++y == 4) {
            outdata[i++] = (unsigned char)((t >> 16) & 0xff);
            if (g > 1) outdata[i++] = (unsigned char)((t >> 8) & 0xff);
            if (g > 2) outdata[i++] = (unsigned char)(t & 0xff);
            y = t = 0;
        }
    }
    if (outlen != NULL) {
        *outlen = i - countlen;
    }
    return ret;
}

/**
 * @brief  下位机固件更新函数
 * @param  *params json对象(类字典)
 * @return 更新中：返回当前接收字节长度；更新错误：返回0
 */
cJSON *update_firmware(cJSON* params)
{
    static uint64_t outlen, countlen = 0, lastremainlen = 0;
    static uint16_t inlen, templen;
    static char *outdata;
    static uint8_t ret = UPDATE_WAIT;
    //读取文件base64总长度,并申请outdata空间
    if (ret == UPDATE_WAIT && params->child->next->valueint != 0) 
    {
        log_i("Firmware update start......");
        log_i("Firmware package downloading");
        outlen = params->child->next->valueint;
        outdata = malloc(sizeof(char) * (outlen));
        ret--;
    }
    else {ret = UPDATE_ERROR; log_e("Total length is zero");}
    //接收分批的base64数据并解码
    if (ret == UPDATING) 
    {   //base64长度,必须为4的倍数
        inlen = strlen(params->child->valuestring);
        if (inlen % 4 != 0) 
        {
            ret = UPDATE_ERROR;
            log_e("Base64 length is not a multiple of 4");
        }
        else //base64解码
        {
            base64_decode(params->child->valuestring, inlen, outdata, &templen, countlen);
            countlen += templen;  //累计已接受长度
            if (outlen == countlen) 
            {
                log_i("Firmware package download success");
                ret--; //是否接收完毕
            }
            else if (lastremainlen != 0 && (params->child->next->valueint - lastremainlen) != templen) 
            {   //校验两次剩余长度差是否为解码后长度
                ret = UPDATE_ERROR; 
                log_e("Decoding length error");
            } 
            else
            {   //接收成功
                lastremainlen = params->child->next->valueint; 
                return cJSON_CreateNumber(templen);
            } 
        }
    }
    //复原文件并更新固件
    if (ret == UPDATE_FINISH)
    {
        //获取更新脚本目录
        char a[50];
        getcwd(a, 50);
        sprintf(a, "%s/update.sh", a);
        //恢复成tar.gz文件
        FILE *fp = fopen("./lib/update.tar.gz", "wb");
        for (int i = 0; i < outlen; i++)
        {
            fputc(outdata[i], fp);
        }
        fclose(fp);
        //运行更新脚本
        if(system(a) == 0) 
        {
            ret = 2;        
            countlen = 0;
            free(outdata);
            log_i("Firmware update success");
            return cJSON_CreateNumber(templen);
        }
        else 
        {
            ret = -1;
            log_e("SH file execution error"); 
        }
    }
    //更新错误
    if (ret == UPDATE_ERROR)
    {
        ret = UPDATE_WAIT;
        countlen = 0;
        free(outdata);
        return cJSON_CreateNumber(0);
    }
}
