#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include <string>
#include <exception>
#include <functional>
#include <easylogging++.h>
#include <nlohmann/json.hpp>

#include "concept.h"
#include "control.h"
#include "utils.h"

using Json = nlohmann::json;

void to_json(Json &j, const PropellerParameters& p);
void from_json(const Json &j, PropellerParameters& p);

void to_json(Json &j, const PropellerGroup& p);
void from_json(const Json &j, PropellerGroup& p);

void to_json(Json &j, const PropellerAttribute& p);
void from_json(const Json &j, PropellerAttribute& p);

void to_json(Json &j, const PIDParameters& p);
void from_json(const Json &j, PIDParameters& p);

void to_json(Json &j, const PWMDeviceParams& p);
void from_json(const Json &j, PWMDeviceParams& p);

class Config 
{
public:
    // void load_from_path(const std::string& path) noexcept;
    void load_from_file(const std::string& file_name) noexcept;

    std::string dump() const noexcept;

    template<typename T>
    void setValue(const std::string& key, const T& value) 
    {
        const Json& old_value = m_datas.at(key);
        m_datas[key] = value;
        auto result = m_cbs.find(key);
        if(result != m_cbs.end())
            result->second(old_value, m_datas.at(key));
    }

    template<typename T>
    T getValue(const std::string& key, const T& default_value, std::function<void(const Json&,const Json&)> cb = nullptr)
    {
        if(cb) {
            m_cbs[key] = cb;
        }
        try 
        {
            T t;
            m_datas.at(key).get_to(t);
            return t;
        } catch (const std::exception& e)
        {
            m_datas[key] = default_value;
            return default_value;
        }
    }

    template<typename T>
    T getValue(const std::string& key)
    {
        try 
        {
            T t;
            m_datas.at(key).get_to(t);
            return t;
        } catch (const std::exception& e)
        {
            LOG(ERROR) << "没有此配置选项 key=" << key;
        }
    }
private:
    std::map<std::string, std::function<void(const Json&,const Json&)>> m_cbs;
    Json m_datas;
};

#endif