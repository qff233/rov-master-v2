#include "config.h"

#include <fstream>

// void Config::load_from_path(const std::string& path) noexcept 
// {

// }

void Config::load_from_file(const std::string& file_path) noexcept 
{
    std::fstream fs(file_path);
    fs >> m_datas;
}

std::string Config::dump() const noexcept 
{
    return m_datas.dump();
}
