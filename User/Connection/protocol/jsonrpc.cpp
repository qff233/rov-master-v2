#include "jsonrpc.h"

#define LOG_TAG "JsonRpc"
#define LOG_LVL ELOG_LVL_INFO

#include <elog.h>

void JsonRpc::parse(const std::string& json_str) 
{
    try
    {
        m_json = Json::parse(json_str.c_str());
    } catch(const std::exception& e)
    {
        log_e(e.what());
    }
}