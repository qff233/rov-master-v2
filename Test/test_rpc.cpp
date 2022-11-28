#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>
#include <elog.h>

#include "User/Connection/rpc_server.h"
#include "User/Connection/method_get_info.h"

int main() 
{
    elog_init();
    /* 设置 EasyLogger 日志格式 */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);

    elog_set_text_color_enabled(true);
    elog_start();
    wiringPiSetup();

    log_i("start");

    RPCServer rpc;
    rpc.addMethod("get_info", jsonrpccxx::GetHandle(Method::get_info));
    rpc.start();
    sleep(100000000);
    return 0;
}
