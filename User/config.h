#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define LOG_TAG "Config"
#define LOG_LVL ELOG_LVL_ERROR

#include <string>
#include <easylogging++.h>
#include <exception>
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

class Config 
{
public:
    Config() noexcept;

    void load_from_path(const std::string& path) noexcept;
    void load_from_file(const std::string& file_name) noexcept;

    std::string to_string() const noexcept;

    template<typename T>
    T getValue(const std::string& key) {
        try 
        {
            
        } catch (const std::exception& e)
        {
            LOG(ERROR) << "没有此配置或类型错误";
            return T{};
        }
    }
private:
    Json m_datas;
};

#endif